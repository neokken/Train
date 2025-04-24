#include "precomp.h"
#include "Game/Trains/Wagon.h"

#include "Camera/Camera.h"
#include "Renderables/Arrow.h"
#include "Renderables/Circle.h"
#include "Renderables/LineSegment.h"
#include "World/World.h"
#include "Train.h"
#include "Locomotive.h"

Train::Train( const std::vector<Wagon*>& wagons, TrackManager& trackManager )
	: GameObject()
	  , m_trackManager(trackManager)
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
			m_wagons[0]->Move(deltaTime * m_velocity, deltaTime);
			if (m_targetDistance > 0) m_targetDistance = max(0.f, m_targetDistance - deltaTime * m_velocity);
			else if (m_targetDistance < 0) m_targetDistance = min(0.f, m_targetDistance - deltaTime * m_velocity);
			for (int i = 1; i < static_cast<int>(m_wagons.size()); ++i)
			{
				TrackWalker& front = m_wagons[i - 1]->GetBackWalker();
				TrackWalker& back = m_wagons[i]->GetFrontWalker();

				float walkerDistance = length(front.GetPosition() - back.GetPosition());
				float diff = walkerDistance - m_wagonSpacing;
				WagonMovementInfo moveInfo = m_wagons[i]->Move(diff, deltaTime);
				velocityChange += moveInfo.velocityChangeAmount;
			}
		}
		else if (m_velocity < 0)
		{
			m_wagons[m_wagons.size() - 1]->Move(deltaTime * m_velocity, deltaTime);
			if (m_targetDistance > 0) m_targetDistance = max(0.f, m_targetDistance - deltaTime * m_velocity);
			else if (m_targetDistance < 0) m_targetDistance = min(0.f, m_targetDistance - deltaTime * m_velocity);
			for (int i = static_cast<int>(m_wagons.size()) - 2; i >= 0; --i)
			{
				TrackWalker& front = m_wagons[i + 1]->GetFrontWalker();
				TrackWalker& back = m_wagons[i]->GetBackWalker();
				float walkerDistance = length(front.GetPosition() - back.GetPosition());
				float diff = m_wagonSpacing - walkerDistance;
				WagonMovementInfo moveInfo = m_wagons[i]->Move(diff, deltaTime);
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
	}
}

void Train::Render( const Engine::Camera& camera, Surface& target )
{
	for (int i = 1; i < static_cast<int>(m_wagons.size()); ++i)
	{
		TrackWalker& front = m_wagons[i - 1]->GetBackWalker();
		TrackWalker& back = m_wagons[i]->GetFrontWalker();
		Engine::LineSegment::RenderWorldPos(camera, target, front.GetPosition(), back.GetPosition(), m_wireColor);
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
	ImGui::NewLine();
	ImGui::InputInt("Target Segment", &targetSegment);
	if (ImGui::Button("Pathfind"))
	{
		SetNavTarget((TrackSegmentID)targetSegment, 0.f);
	}
}

void Train::VisualizeDebugInfo( const Engine::Camera& camera, Surface& screen, Engine::World& world ) const
{
	TrackWalker tempWalker;
	tempWalker.Init(&world.GetTrackManager());

	//Target
	if (m_targetDistance > 0)
	{
		tempWalker.SetCurrentTrackSegment(m_wagons[0]->GetFrontWalker().GetCurrentTrackSegment(), m_wagons[0]->GetFrontWalker().GetDistance());
		tempWalker.Move(m_targetDistance);
		Engine::Circle::RenderWorldPos(camera, screen, tempWalker.GetPosition(), 1.2f, 0xff00ff);
	}
	else if (m_targetDistance < 0)
	{
		tempWalker.SetCurrentTrackSegment(m_wagons[m_wagons.size() - 1]->GetBackWalker().GetCurrentTrackSegment(), m_wagons[m_wagons.size() - 1]->GetBackWalker().GetDistance());
		tempWalker.Move(m_targetDistance);
		Engine::Circle::RenderWorldPos(camera, screen, tempWalker.GetPosition(), 1.2f, 0xff00ff);
	}

	//Stopping distance
	float dist = GetMaxStoppingDistance();
	if (m_velocity >= 0)
	{
		tempWalker.SetCurrentTrackSegment(m_wagons[0]->GetFrontWalker().GetCurrentTrackSegment(), m_wagons[0]->GetFrontWalker().GetDistance());
		tempWalker.Move(dist);
	}
	else
	{
		tempWalker.SetCurrentTrackSegment(m_wagons[m_wagons.size() - 1]->GetBackWalker().GetCurrentTrackSegment(), m_wagons[m_wagons.size() - 1]->GetBackWalker().GetDistance());
		tempWalker.Move(-dist);
	}

	Engine::Circle::RenderWorldPos(camera, screen, tempWalker.GetPosition(), 1.f, 0xff0000);
}

void Train::SetNavTarget( const TrackSegmentID segment, const float distanceOnSegment )
{
	m_targetSegment = segment;
	m_targetDistanceOnTargetSegment = distanceOnSegment;
	const bool towardsB = true;
	TrackSegmentID curr = m_wagons[0]->GetFrontWalker().GetCurrentTrackSegment();
	m_currentPath = m_trackManager.CalculatePath(curr, towardsB, m_targetSegment);

	//Get path distance (in the future this should be the distance until the first blocked signal)
	TrackNodeID currentNode = towardsB ? m_trackManager.GetTrackSegment(curr).nodeB : m_trackManager.GetTrackSegment(curr).nodeA;
	TrackSegmentID currentSegment = curr;
	float currentDistance = m_wagons[0]->GetFrontWalker().GetDistance();
	float distance = m_trackManager.GetTrackSegment(curr).distance - currentDistance;

	for (int i = 0; i < m_currentPath.size(); ++i)
	{
		m_trackManager.SetNodeLever(currentNode, currentSegment, m_currentPath[i]);
		currentSegment = m_trackManager.GetTrackNode(currentNode).validConnections.at(currentSegment)[m_currentPath[i]];
		const TrackSegment& seg = m_trackManager.GetTrackSegment(currentSegment);
		distance += seg.distance;
		if (seg.nodeA == currentNode) currentNode = seg.nodeB;
		else currentNode = seg.nodeA;
	}
	m_targetDistance = distance;
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
