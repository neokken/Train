#include "precomp.h"
#include "Game/Trains/Wagon.h"

#include "Camera/Camera.h"
#include "Renderables/Arrow.h"
#include "Renderables/Circle.h"
#include "Renderables/LineSegment.h"
#include "World/World.h"
#include "Train.h"
#include "Locomotive.h"

#include <algorithm>
#include "Locomotive.h"

Train::Train( const std::vector<Wagon*>& wagons, TrackManager& trackManager, SignalManager& signalManager )
	: GameObject()
	  , m_trackManager(trackManager)
	  , m_signalManager(&signalManager)
{
	for (auto wagon : wagons)
	{
		m_wagons.emplace_back(wagon);
	}

	float currDistance = 0;
	m_wagons[0]->SetLocked(true);

	for (int i = 1; i < static_cast<int>(m_wagons.size()); ++i)
	{
		m_wagons[i]->SetLocked(true);
		m_wagons[i]->SetInvincible(true);
		m_wagons[i]->GetFrontWalker().SetCurrentTrackSegment(m_wagons[0]->GetFrontWalker().GetCurrentTrackSegment(), m_wagons[0]->GetFrontWalker().GetDistance());
		currDistance += (m_wagons[i]->GetWagonLength()) + m_wagonSpacing;
		m_wagons[i]->Move(-currDistance, 0);
		TrackWalker& front = m_wagons[i - 1]->GetBackWalker();
		TrackWalker& back = m_wagons[i]->GetFrontWalker();
		for (int j = 0; j < 5; ++j) // Settle in to its position
		{
			float walkerDistance = length(front.GetPosition() - back.GetPosition());
			float diff = walkerDistance - m_wagonSpacing;
			m_wagons[i]->Move(diff, 0);
		}
		m_wagons[i]->SetInvincible(false);
	}

	CalculateWagons();
}

void Train::Update( const float deltaTime )
{
	if (m_wagons.empty()) return;

	//Pathfinding
	if (!m_currentPath.empty() || m_targetDistance > 0.f || (m_upcomingSignal != SignalID::Invalid)) CheckPathAvailability();
	if (m_targetDistance > 0)
	{
		if (m_targetDistance < GetMaxStoppingDistance())
		{
			m_targetVelocity = 0.f;
		}
		else
		{
			m_targetVelocity = 1000000.f; // TODO: implement max speed track rules or something
		}
	}
	else if (m_targetDistance < 0)
	{
		if (-m_targetDistance < GetMaxStoppingDistance())
		{
			m_targetVelocity = 0.f;
		}
		else
		{
			m_targetVelocity = -1000000.f; // TODO: implement max speed track rules or something
		}
	}

	//Set acceleration
	m_braking = false;
	if (m_targetVelocity == 0.f) // Braking
	{
		m_braking = true;
	}
	else if (m_targetVelocity > 0.f)
	{
		if (m_velocity < m_targetVelocity)
		{
			if (m_velocity < 0) m_braking = true;
			else m_acceleration = m_maxAccelerationForward;
		}
		else m_acceleration = 0.f;
	}
	else if (m_targetVelocity < 0.f)
	{
		if (m_velocity > m_targetVelocity)
		{
			if (m_velocity > 0) m_braking = true;
			else m_acceleration = -m_maxAccelerationBackwards;
		}
		else m_acceleration = 0.f;
	}
	//Physics calculations

	if (m_braking) // When braking stop engines
	{
		if (m_velocity != 0.f)
		{
			m_acceleration = 0.f;
			float brakeForce = m_velocity > 0 ? m_maxBrakingForce : -m_maxBrakingForce;
			float newVelocity = m_velocity - brakeForce / m_mass * deltaTime;
			if ((m_velocity > 0 && newVelocity < 0) || (m_velocity < 0 && newVelocity > 0)) // Fix overcorrecting
			{
				m_velocity = 0.0f;
			}
			else
			{
				m_velocity = newVelocity;
			}
		}
	}
	else
	{
		m_velocity += (m_acceleration / m_mass) * deltaTime; // Technically frame dependent but good enough
	}
	if (m_velocity != 0.f)
	{
		float airDragCoefficient = m_velocity > 0 ? m_wagons[0]->GetAirDragCoefficient() : m_wagons[m_wagons.size() - 1]->GetAirDragCoefficient();
		float drag = airDragCoefficient * WORLD_AIR_DENSITY * (m_velocity * m_velocity) * 0.5f;
		drag = m_velocity > 0 ? drag : -drag;
		float newVelocity = m_velocity - drag / m_mass * deltaTime; // Technically frame dependent but good enough
		if ((m_velocity > 0 && newVelocity < 0) || (m_velocity < 0 && newVelocity > 0)) // Fix overcorrecting
		{
			m_velocity = 0.0f;
		}
		else
		{
			m_velocity = newVelocity;
		}

		float velocityChange = 0.f;
		if (m_velocity > 0)
		{
			const TrackSegment& segment = m_trackManager.GetTrackSegment(m_wagons[0]->GetFrontWalker().GetCurrentTrackSegment());
			float oldDist = m_wagons[0]->GetFrontWalker().GetDistance() / segment.distance;
			m_wagons[0]->Move(deltaTime * m_velocity, deltaTime);
			float newDist = m_wagons[0]->GetFrontWalker().GetDistance() / segment.distance;
			//Check if it passed a signal
			if (newDist < oldDist) newDist = 1.01f; // In case moved to new segment
			for (SignalID signalID : segment.signals)
			{
				const auto& signal = m_signalManager->GetSignal(signalID);

				if (signal.blockInFront != SignalBlockID::Invalid
					&& signal.directionTowardsNodeB == GetDirectionOnTrack()
					&& InRange(signal.distanceOnSegment, oldDist, newDist)
				)
				{
					m_signalManager->EnterBlock(signal.blockInFront, m_id);
					if (!m_pathSignalsRaw.empty() && m_pathSignalsRaw[0] == signalID)
					{
						m_pathSignalsRaw.erase(m_pathSignalsRaw.begin());
						m_pathSignals[0].erase(m_pathSignals[0].begin());
					}
				}
			}


			if (m_targetDistance > 0) m_targetDistance = max(0.f, m_targetDistance - deltaTime * m_velocity);
			else if (m_targetDistance < 0) m_targetDistance = min(0.f, m_targetDistance - deltaTime * m_velocity);
			for (int i = 1; i < static_cast<int>(m_wagons.size()); ++i)
			{
				TrackWalker& front = m_wagons[i - 1]->GetBackWalker();
				TrackWalker& back = m_wagons[i]->GetFrontWalker();

				float walkerDistance = length(front.GetPosition() - back.GetPosition());
				float diff = walkerDistance - m_wagonSpacing;

				const TrackSegment& backsegment = m_trackManager.GetTrackSegment(m_wagons[i]->GetBackWalker().GetCurrentTrackSegment());

				float oldDistBack = m_wagons[i]->GetBackWalker().GetDistance() / backsegment.distance;
				WagonMovementInfo moveInfo = m_wagons[i]->Move(diff, deltaTime);
				if (i == static_cast<int>(m_wagons.size()) - 1)
				{
					float newDistBack = m_wagons[i]->GetBackWalker().GetDistance() / backsegment.distance;
					//Check if it passed a signal
					for (SignalID signalID : backsegment.signals)
					{
						const auto& signal = m_signalManager->GetSignal(signalID);
						if (signal.blockBehind != SignalBlockID::Invalid
							&& signal.directionTowardsNodeB == GetDirectionOnTrack()
							&& InRange(signal.distanceOnSegment, oldDistBack, newDistBack)
						)
						{
							m_signalManager->ExitBlock(signal.blockBehind, m_id);
						}
					}
				}
				velocityChange += moveInfo.velocityChangeAmount;
			}
		}
		else if (m_velocity < 0)
		{
			//Flip the direction of movement if back wagon isnt aligned with front wagon direction
			const int flipDirection = (m_wagons[m_wagons.size() - 1]->GetDirectionOnTrack() != GetDirectionOnTrack()) ? -1 : 0;

			const TrackSegment& segment = m_trackManager.GetTrackSegment(m_wagons[m_wagons.size() - 1]->GetBackWalker().GetCurrentTrackSegment());
			float oldDist = m_wagons[m_wagons.size() - 1]->GetBackWalker().GetDistance() / segment.distance;
			m_wagons[m_wagons.size() - 1]->Move(deltaTime * m_velocity * flipDirection, deltaTime);
			float newDist = m_wagons[m_wagons.size() - 1]->GetBackWalker().GetDistance() / segment.distance;
			//Check if it passed a signal
			if (newDist > oldDist) newDist = 0.01f; // In case moved to new segment
			for (SignalID signalID : segment.signals)
			{
				const auto& signal = m_signalManager->GetSignal(signalID);

				if (signal.blockInFront != SignalBlockID::Invalid
					&& signal.directionTowardsNodeB == !GetDirectionOnTrack()
					&& InRange(signal.distanceOnSegment, oldDist, newDist)
				)
				{
					m_signalManager->EnterBlock(signal.blockInFront, m_id);
				}
			}



			if (m_targetDistance > 0) m_targetDistance = max(0.f, m_targetDistance - deltaTime * m_velocity);
			else if (m_targetDistance < 0) m_targetDistance = min(0.f, m_targetDistance - deltaTime * m_velocity);
			for (int i = static_cast<int>(m_wagons.size()) - 2; i >= 0; --i)
			{
				TrackWalker& front = m_wagons[i + 1]->GetFrontWalker();
				TrackWalker& back = m_wagons[i]->GetBackWalker();
				float walkerDistance = length(front.GetPosition() - back.GetPosition());
				float diff = m_wagonSpacing - walkerDistance;
				const TrackSegment& segment = m_trackManager.GetTrackSegment(m_wagons[i]->GetFrontWalker().GetCurrentTrackSegment());
				float oldDist = m_wagons[i]->GetFrontWalker().GetDistance() / segment.distance;
				WagonMovementInfo moveInfo = m_wagons[i]->Move(diff, deltaTime);
				if (i == 0)
				{
					float newDist = m_wagons[i]->GetFrontWalker().GetDistance() / segment.distance;
					//Check if it passed a signal
					for (SignalID signalID : segment.signals)
					{
						const auto& signal = m_signalManager->GetSignal(signalID);

						if (signal.blockBehind != SignalBlockID::Invalid
							&& signal.directionTowardsNodeB == !GetDirectionOnTrack()
							&& InRange(signal.distanceOnSegment, oldDist, newDist)
						)
						{
							m_signalManager->ExitBlock(signal.blockBehind, m_id);
						}
					}
				}
				velocityChange += moveInfo.velocityChangeAmount;
			}
		}

		float trackDrag = WORLD_TRACK_ROUGHNESS * (velocityChange);
		newVelocity = m_velocity - trackDrag / m_mass * deltaTime; // Technically frame dependent but good enough
		if ((m_velocity > 0 && newVelocity < 0) || (m_velocity < 0 && newVelocity > 0)) // Fix overcorrecting
		{
			m_velocity = 0.0f;
		}
		else
		{
			m_velocity = newVelocity;
		}

		//Update path
		if (m_targetDistance > 0)
		{
			auto iter = ranges::find_if(m_currentPath,
			                            [this]( const std::pair<TrackSegmentID, int>& element ) { return element.first == m_wagons[0]->GetFrontWalker().GetCurrentTrackSegment(); });
			if (iter != m_currentPath.end())
			{
				m_currentPath.erase(m_currentPath.begin(), std::next(iter));
				m_pathSignals.erase(m_pathSignals.begin());
			}
		}
	}
}

void Train::Render( const Engine::Camera& camera )
{
	for (int i = 1; i < static_cast<int>(m_wagons.size()); ++i)
	{
		TrackWalker& front = m_wagons[i - 1]->GetBackWalker();
		TrackWalker& back = m_wagons[i]->GetFrontWalker();
		Engine::LineSegment::RenderWorldPos(camera, front.GetPosition(), back.GetPosition(), m_wireColor);
	}
}

void Train::ImGuiDebugViewer()
{
	if (ImGui::CollapsingHeader("Transform"))
	{
		if (ImGui::TreeNode("Info"))
		{
			ImGui::Text("These values are not changeable since they are reset/recalculated every frame");
			ImGui::Text("if you wish to set the position. Look into getting access to the trackwalker");
			ImGui::Text("and setting the currentTrackID and distance..");
			ImGui::TreePop();
		}

		ImGui::Text("Position: %.2f %.2f", m_transform.position.x, m_transform.position.y);
		ImGui::Text("Scale: %.2f %.2f", m_transform.scale.x, m_transform.scale.y);
	}

	ImGui::DragFloat("Target Distance", &m_targetDistance, .1f);
	ImGui::DragFloat("Target Velocity", &m_targetVelocity, .1f);
	ImGui::DragFloat("Velocity", &m_velocity, .1f);
	ImGui::DragFloat("Acceleration", &m_acceleration, .1f);
	ImGui::Text(("Max Acceleration:" + std::to_string(static_cast<int>(m_maxAccelerationBackwards)) + " <-> " + std::to_string(static_cast<int>(m_maxAccelerationForward))).c_str());
	ImGui::Text(("BrakeForce :" + std::to_string(static_cast<int>(m_maxBrakingForce))).c_str());
	ImGui::Text(("Current mass:" + std::to_string(m_mass)).c_str());
}

void Train::VisualizeDebugInfo( const Engine::Camera& camera, Engine::World& world ) const
{
	TrackWalker tempWalker;
	tempWalker.Init(&world.GetTrackManager());

	//Target
	if (m_targetDistance > 0)
	{
		int flipDir = (GetDirectionOnTrack() != tempWalker.GetTrackDirection()) ? -1 : 1;
		tempWalker.SetCurrentTrackSegment(m_wagons[0]->GetFrontWalker().GetCurrentTrackSegment(), m_wagons[0]->GetFrontWalker().GetDistance());
		tempWalker.Move(m_targetDistance * static_cast<float>(flipDir));

		Engine::Circle::RenderWorldPos(camera, tempWalker.GetPosition(), 1.2f, 0xff00ff);
	}
	else if (m_targetDistance < 0)
	{
		int flipDir = (m_wagons[m_wagons.size() - 1]->GetBackWalker().GetTrackDirection() != tempWalker.GetTrackDirection()) ? -1 : 1;
		tempWalker.SetCurrentTrackSegment(m_wagons[m_wagons.size() - 1]->GetBackWalker().GetCurrentTrackSegment(), m_wagons[m_wagons.size() - 1]->GetBackWalker().GetDistance());
		tempWalker.Move(m_targetDistance * static_cast<float>(flipDir));

		Engine::Circle::RenderWorldPos(camera, tempWalker.GetPosition(), 1.2f, 0xff00ff);
	}

	//Stopping distance
	float dist = GetMaxStoppingDistance();
	if (m_velocity >= 0)
	{
		int flipDir = (GetDirectionOnTrack() != tempWalker.GetTrackDirection()) ? -1 : 1;
		tempWalker.SetCurrentTrackSegment(m_wagons[0]->GetFrontWalker().GetCurrentTrackSegment(), m_wagons[0]->GetFrontWalker().GetDistance());
		tempWalker.Move(dist * static_cast<float>(flipDir));
	}
	else
	{
		int flipDir = (m_wagons[m_wagons.size() - 1]->GetBackWalker().GetTrackDirection() != tempWalker.GetTrackDirection()) ? -1 : 1;
		tempWalker.SetCurrentTrackSegment(m_wagons[m_wagons.size() - 1]->GetBackWalker().GetCurrentTrackSegment(), m_wagons[m_wagons.size() - 1]->GetBackWalker().GetDistance());
		tempWalker.Move(dist * static_cast<float>(flipDir));

	}

	Engine::Circle::RenderWorldPos(camera, tempWalker.GetPosition(), 1.f, 0xff0000);
}

void Train::SetNavTarget( const TrackSegmentID segment, const float distanceOnSegment )
{
	m_targetSegment = segment;
	m_targetDistanceOnTargetSegment = distanceOnSegment;
	const bool towardsB = GetDirectionOnTrack();
	TrackSegmentID curr = m_wagons[0]->GetFrontWalker().GetCurrentTrackSegment();
	std::vector<int> path = m_trackManager.CalculatePath(curr, towardsB, m_targetSegment);

	//Get path distance
	TrackNodeID currentNode = towardsB ? m_trackManager.GetTrackSegment(curr).nodeB : m_trackManager.GetTrackSegment(curr).nodeA;
	TrackSegmentID currentSegment = curr;
	float currentDistance = m_wagons[0]->GetFrontWalker().GetDistance();
	float distance = m_trackManager.GetTrackSegment(curr).distance - currentDistance;
	if (path.empty())
	{
		distance = distanceOnSegment - currentDistance;
	}
	m_pathSignals = m_signalManager->GetPathSignals(path, curr, towardsB, currentDistance / m_trackManager.GetTrackSegment(curr).distance);
	m_pathSignalsRaw.clear();
	m_currentPath.clear();
	m_upcomingSignal = SignalID::Invalid;
	m_pathSignalsRaw.insert(m_pathSignalsRaw.begin(), m_pathSignals[0].begin(), m_pathSignals[0].end());
	for (int i = 0; i < path.size(); ++i)
	{
		m_pathSignalsRaw.insert(m_pathSignalsRaw.end(), m_pathSignals[i + 1].begin(), m_pathSignals[i + 1].end());
		m_trackManager.SetNodeLever(currentNode, currentSegment, path[i]);
		currentSegment = m_trackManager.GetTrackNode(currentNode).validConnections.at(currentSegment)[path[i]];
		m_currentPath.push_back(std::pair(currentSegment, path[i]));
		const TrackSegment& seg = m_trackManager.GetTrackSegment(currentSegment);
		if (i == static_cast<int>(path.size()) - 1)
		{
			if (currentNode == seg.nodeA) distance += seg.distance * distanceOnSegment;
			else distance += seg.distance * (1.f - distanceOnSegment);
		}
		else
		{
			distance += seg.distance;
		}
		if (seg.nodeA == currentNode) currentNode = seg.nodeB;
		else currentNode = seg.nodeA;
	}
	m_targetDistance = distance;
	CheckPathAvailability();
}

bool Train::GetDirectionOnTrack() const
{
	return m_wagons[0]->GetDirectionOnTrack();
}

float Train::GetMaxStoppingDistance() const
{
	float stopForce = m_maxBrakingForce;
	const float airDragCoefficient = m_velocity > 0 ? m_wagons[0]->GetAirDragCoefficient() : m_wagons[m_wagons.size() - 1]->GetAirDragCoefficient();
	stopForce += (0.5f * airDragCoefficient * WORLD_AIR_DENSITY * (m_velocity * 0.5f) * (m_velocity * 0.5f));

	stopForce /= m_mass;
	const float stopTime = m_velocity / stopForce;
	const float distance = stopTime * m_velocity * 0.5f;

	return distance;
}

void Train::CalculateWagons()
{
	float forwardAccel{0.f};
	float backwardAccel{0.f};
	float brakingForce{0.f};
	float mass{0.f};
	for (Wagon* wagon : m_wagons)
	{
		//TODO: Take into account wagon orientation
		if (const Locomotive* locomotive = dynamic_cast<Locomotive*>(wagon))
		{
			forwardAccel += locomotive->GetMaxForwardAcceleration();
			backwardAccel += locomotive->GetMaxBackwardsAcceleration();
		}
		brakingForce += wagon->GetBrakingForce();
		mass += wagon->GetMass();
	}
	m_maxAccelerationForward = forwardAccel;
	m_maxAccelerationBackwards = backwardAccel;
	m_maxBrakingForce = brakingForce;
	m_mass = mass;
}

void Train::CheckPathAvailability()
{
	if (m_pathSignalsRaw.empty()) return;
	std::vector<SignalID> nextSignals;
	if (m_pathSignalsRaw.size() > 1) nextSignals.insert(nextSignals.begin(), std::next(m_pathSignalsRaw.begin()), m_pathSignalsRaw.end());
	if (m_upcomingSignal != SignalID::Invalid)
	{
		if (!m_signalManager->CanPassSignal(m_upcomingSignal, nextSignals, m_id))
		{
			return;
		}
		m_upcomingSignal = SignalID::Invalid;
	}
	TrackSegmentID curr = m_wagons[0]->GetFrontWalker().GetCurrentTrackSegment();
	const TrackSegment& segment = m_trackManager.GetTrackSegment(curr);
		float currentDistance = m_wagons[0]->GetFrontWalker().GetDistance();
	float distance = segment.distance - currentDistance;
	if (segment.id == m_targetSegment)
	{
		distance = m_targetDistanceOnTargetSegment * segment.distance - currentDistance;
	}
	std::vector<SignalID> signals = m_pathSignalsRaw;

	for (const SignalID signal : m_pathSignals.at(0))
	{
		signals.erase(signals.begin());
		if (!m_signalManager->CanPassSignal(signal, nextSignals, m_id))
		{
			distance = m_signalManager->GetSignal(signal).distanceOnSegment * segment.distance - currentDistance;
			m_targetDistance = distance - 5.f; // Small offset for safety
			m_upcomingSignal = signal;
			return;
		}
	}

	for (int i = 0; i < m_currentPath.size(); ++i)
		{
			for (const SignalID signal : m_pathSignals.at(i + 1))
			{
				signals.erase(signals.begin());
				if (!m_signalManager->CanPassSignal(signal, nextSignals, m_id))
				{
					distance += m_signalManager->GetSignal(signal).distanceOnSegment * m_trackManager.GetTrackSegment(m_currentPath[i].first).distance;
					m_targetDistance = distance - 5.f; // Small offset for safety
					m_upcomingSignal = signal;
					return;
				}
			}
			distance += m_trackManager.GetTrackSegment(m_currentPath[i].first).distance;
		}
		m_targetDistance = distance;
		m_upcomingSignal = SignalID::Invalid;
}
