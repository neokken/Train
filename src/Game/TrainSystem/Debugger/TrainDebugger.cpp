#include "precomp.h"
#include "TrainDebugger.h"

#include <imgui_internal.h>

#include "Camera/Camera.h"
#include "Game/Trains/Train.h"
#include "Game/Trains/Wagon.h"
#include "Game/TrainSystem/TrackBuilder.h"
#include "Game/TrainSystem/TrackManager.h"
#include "Helpers/Renderer.h"
#include "Renderables/Circle.h"
#include "Renderables/CurvedSegment.h"
#include "Renderables/LineSegment.h"
#include "UI/UIManager.h"

void TrainDebugger::Init( TrackManager& trackManager, TrainManager& trainManager, TrackBuilder& trackBuilder )
{
	m_trackManager = &trackManager;
	m_trainManager = &trainManager;
	m_trackBuilder = &trackBuilder;
}

void TrainDebugger::Update( const Engine::Camera& camera )
{
	const Engine::InputManager& input = Input::get();

	const float2 worldPosMouse = camera.GetWorldPosition(input.GetMousePos());

	if (!m_trackBuilder->InBuildMode() && !input.IsKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		if (input.IsMouseClicked(0) && !Engine::IsMouseOverUI())
		{
			m_hasSelectedTrain = false;
		}
		if (!m_hasSelectedTrain)
		{
			m_selectedTrain = TrainID::Invalid;
			auto trainMap = m_trainManager->GetTrainMap();
			for (auto train : trainMap)
			{
				for (auto wagon : train.second->GetWagons())
				{
					if (length(wagon->GetTransform().position - worldPosMouse) < 3.f)
					{
						//Engine::Circle::RenderWorldPos(camera, wagon->GetTransform().position, 2.f, 0x3030ff);
						m_selectedTrain = train.first;
						if (input.IsMouseClicked(0))
						{
							m_hasSelectedTrain = true;
						}
					}
				}
			}
		}
	}

	//Train path visualisation
	if (m_selectedTrain != TrainID::Invalid)
	{
		TrackWalker tempWalker;
		tempWalker.Init(m_trackManager);

		const Train& train = m_trainManager->GetTrain(m_selectedTrain);
		float targetDist = train.GetTargetDistanceRemaining();
		if (targetDist > 0)
		{
			//Show destination
			if (targetDist > 0)
			{
				int flipDir = (train.GetDirectionOnTrack() != tempWalker.GetTrackDirection()) ? -1 : 1;
				tempWalker.SetCurrentTrackSegment(train.GetWagons()[0]->GetFrontWalker().GetCurrentTrackSegment(), train.GetWagons()[0]->GetFrontWalker().GetDistance());
				tempWalker.Move(targetDist * static_cast<float>(flipDir));
				Engine::Circle::RenderWorldPos(camera, tempWalker.GetPosition(), 1.2f, 0xff00ff);
			}
			else if (targetDist < 0)
			{
				int flipDir = (train.GetWagons()[train.GetWagons().size() - 1]->GetBackWalker().GetTrackDirection() != tempWalker.GetTrackDirection()) ? -1 : 1;
				tempWalker.SetCurrentTrackSegment(train.GetWagons()[train.GetWagons().size() - 1]->GetBackWalker().GetCurrentTrackSegment(), train.GetWagons()[train.GetWagons().size() - 1]->GetBackWalker().GetDistance());
				tempWalker.Move(targetDist * static_cast<float>(flipDir));
				Engine::Circle::RenderWorldPos(camera, tempWalker.GetPosition(), 1.2f, 0xff00ff);
			}

			//Show path
			TrackSegmentID currentSegment = m_trainManager->GetTrain(m_selectedTrain).GetWagons()[0]->GetFrontWalker().GetCurrentTrackSegment();
			bool towardsB = m_trainManager->GetTrain(m_selectedTrain).GetDirectionOnTrack();
			std::vector<std::pair<TrackSegmentID, int>> path = train.GetCurrentPath();
			Engine::CurveData curve;
			TrackSegment currentSeg = m_trackManager->GetTrackSegment(currentSegment);
			TrackNodeID currentNode = towardsB ? currentSeg.nodeA : currentSeg.nodeB;

			for (int i = 0; i < static_cast<int>(path.size()); ++i)
			{
				currentSegment = path[i].first;
				currentSeg = m_trackManager->GetTrackSegment(currentSegment);
				if (currentSeg.nodeA == currentNode)
				{
					currentNode = currentSeg.nodeB;
					curve = {currentSeg.nodeA_Position, currentSeg.nodeA_Direction, currentSeg.nodeB_Position, currentSeg.nodeB_Direction};
					float2 range = (i == static_cast<int>(path.size()) - 1) ? float2(0.f, m_targetDistance) : float2(0.f, 1.f);
					curve.baseSegments = (static_cast<uint>(currentSeg.distance));

					if (i == 0) range.x = m_trainManager->GetTrain(m_selectedTrain).GetWagons()[0]->GetFrontWalker().GetDistance() / currentSeg.distance;
					Engine::CurvedSegment::RenderArrowsWorldPos(camera, curve, 0x0000ff, 0.7f, range);
				}
				else
				{
					currentNode = currentSeg.nodeA;
					curve = {currentSeg.nodeB_Position, currentSeg.nodeB_Direction, currentSeg.nodeA_Position, currentSeg.nodeA_Direction};
					float2 range = (i == static_cast<int>(path.size()) - 1) ? float2(1.f - m_targetDistance, 1.f) : float2(0.f, 1.f);
					if (i == 0) range.x = 1.f - m_trainManager->GetTrain(m_selectedTrain).GetWagons()[0]->GetFrontWalker().GetDistance() / currentSeg.distance;
					curve.baseSegments = (static_cast<uint>(currentSeg.distance));
					Engine::CurvedSegment::RenderArrowsWorldPos(camera, curve, 0x0000ff, 0.7f, range);
				}
			}
		}

		if (m_showBrakeDistance)
		{
			if (train.GetVelocity() != 0.f)
			{
				//Stopping distance
				float dist = train.GetMaxStoppingDistance();
				if (train.GetVelocity() > 0)
				{
					int flipDir = (train.GetDirectionOnTrack() != tempWalker.GetTrackDirection()) ? -1 : 1;
					tempWalker.SetCurrentTrackSegment(train.GetWagons()[0]->GetFrontWalker().GetCurrentTrackSegment(), train.GetWagons()[0]->GetFrontWalker().GetDistance());
					tempWalker.Move(dist * static_cast<float>(flipDir));
				}
				else
				{
					int flipDir = (train.GetWagons()[train.GetWagons().size() - 1]->GetBackWalker().GetTrackDirection() != tempWalker.GetTrackDirection()) ? -1 : 1;
					tempWalker.SetCurrentTrackSegment(train.GetWagons()[train.GetWagons().size() - 1]->GetBackWalker().GetCurrentTrackSegment(), train.GetWagons()[train.GetWagons().size() - 1]->GetBackWalker().GetDistance());
					tempWalker.Move(dist * static_cast<float>(flipDir));
				}
				float2 pos = tempWalker.GetPosition();
				float2 dir = tempWalker.GetDirection();
				Engine::Renderer::GetRenderer().DrawLine({float3(camera.GetCameraPosition(pos + float2(-dir.y, dir.x)), HeightLayer::Debug), float3(camera.GetCameraPosition(pos + float2(dir.y, -dir.x)), HeightLayer::Debug), Engine::RGB8ToRGB32(0x700000), camera.GetZoomLevel() * 0.25f});
			}
		}
	}

	//Train manual control
	if (m_selectedTrain != TrainID::Invalid
		&& m_hasSelectedTrain
		&& !m_trackBuilder->InBuildMode()
		&& input.IsKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		m_targetSegment = m_trackManager->GetClosestTrackSegment(worldPosMouse);
		if (m_targetSegment != TrackSegmentID::Invalid)
		{
			float targetDist = m_trackManager->GetClosestDistanceOnTrackSegment(m_targetSegment, worldPosMouse);
			const TrackSegment& segment = m_trackManager->GetTrackSegment(m_targetSegment);
			Engine::CurveData curve = {segment.nodeA_Position, segment.nodeA_Direction, segment.nodeB_Position, segment.nodeB_Direction};
			Engine::Circle::RenderWorldPos(camera, Engine::CurvedSegment::GetPositionOnCurvedSegment(targetDist, curve), 1.f, 0xff00ff);

			//Show path
			TrackSegmentID currentSegment = m_trainManager->GetTrain(m_selectedTrain).GetWagons()[0]->GetFrontWalker().GetCurrentTrackSegment();
			bool towardsB = m_trainManager->GetTrain(m_selectedTrain).GetDirectionOnTrack();
			std::vector<int> path = m_trackManager->CalculatePath(currentSegment, towardsB, m_targetSegment);
			TrackNodeID currentNode = towardsB ? m_trackManager->GetTrackSegment(currentSegment).nodeB : m_trackManager->GetTrackSegment(currentSegment).nodeA;

			TrackSegment currentSeg = m_trackManager->GetTrackSegment(currentSegment);
			if (!(path.empty() && currentSegment != m_targetSegment))
			{
				if (towardsB)
				{
					curve = {currentSeg.nodeA_Position, currentSeg.nodeA_Direction, currentSeg.nodeB_Position, currentSeg.nodeB_Direction};
					float2 range = float2(m_trainManager->GetTrain(m_selectedTrain).GetWagons()[0]->GetFrontWalker().GetDistance() / currentSeg.distance, 1.f);
					if (path.empty()) range.y = targetDist;
					curve.baseSegments = static_cast<uint>(currentSeg.distance * 2.f) * (1 / (1.f - (range.y - range.x)));
					Engine::CurvedSegment::RenderArrowsWorldPos(camera, curve, 0xff0000, 0.7f, range);
				}
				else
				{
					curve = {currentSeg.nodeB_Position, currentSeg.nodeB_Direction, currentSeg.nodeA_Position, currentSeg.nodeA_Direction};
					float2 range = float2(1.f - m_trainManager->GetTrain(m_selectedTrain).GetWagons()[0]->GetFrontWalker().GetDistance() / currentSeg.distance, 1.f);
					if (path.empty()) range.y = 1.f - targetDist;
					curve.baseSegments = static_cast<uint>(currentSeg.distance * 2.f) * (1 / (1.f - (range.y - range.x)));
					Engine::CurvedSegment::RenderArrowsWorldPos(camera, curve, 0xff0000, 0.7f, range);
				}

				for (int i = 0; i < static_cast<int>(path.size()); ++i)
				{
					currentSegment = m_trackManager->GetTrackNode(currentNode).validConnections.at(currentSegment)[path[i]];
					currentSeg = m_trackManager->GetTrackSegment(currentSegment);
					if (currentSeg.nodeA == currentNode)
					{
						currentNode = currentSeg.nodeB;
						curve = {currentSeg.nodeA_Position, currentSeg.nodeA_Direction, currentSeg.nodeB_Position, currentSeg.nodeB_Direction};
						float2 range = (i == static_cast<int>(path.size()) - 1) ? float2(0.f, targetDist) : float2(0.f, 1.f);
						curve.baseSegments = (static_cast<uint>(currentSeg.distance));

						Engine::CurvedSegment::RenderArrowsWorldPos(camera, curve, 0xff0000, 0.7f, range);
					}
					else
					{
						currentNode = currentSeg.nodeA;
						curve = {currentSeg.nodeB_Position, currentSeg.nodeB_Direction, currentSeg.nodeA_Position, currentSeg.nodeA_Direction};
						float2 range = (i == static_cast<int>(path.size()) - 1) ? float2(0.f, 1.f - targetDist) : float2(0.f, 1.f);
						curve.baseSegments = (static_cast<uint>(currentSeg.distance));
						Engine::CurvedSegment::RenderArrowsWorldPos(camera, curve, 0xff0000, 0.7f, range);
					}
				}
				if (input.IsMouseClicked(0))
			{
				m_targetDistance = targetDist;
				const Train& train = m_trainManager->GetTrain(m_selectedTrain);
				if (m_targetSegment == train.GetWagons()[0]->GetFrontWalker().GetCurrentTrackSegment())
				{
					m_targetDistance -= (train.GetWagons()[0]->GetFrontWalker().GetDistance() / segment.distance);
				}
				m_trainManager->SetTrainDestination(m_selectedTrain, m_targetSegment, m_targetDistance);
			}
			}
		}
	}
}

void TrainDebugger::UI( const float deltaTime )
{
	if (m_selectedTrain != TrainID::Invalid && m_hasSelectedTrain)
	{
		bool open = true;
		Engine::UIManager::BeginGameplayWindow(("Train: " + std::to_string(static_cast<int>(m_selectedTrain))).c_str(), &open);
		ImGui::Text("Train");
		const Train& train = m_trainManager->GetTrain(m_selectedTrain);
		if (ImGui::TreeNode("Statistics"))
		{
			float2 enginePower = train.GetEnginePower();
			ImGui::Text(("Engine power: " + std::format("< {:.0f}N || {:.0f}N >  ", enginePower.x, enginePower.y)).c_str());
			ImGui::Text(("Braking power: " + std::format("{:.0f}N", train.GetBrakePower())).c_str());
			float mass = train.GetMass();
			if (mass < 1.f)
				ImGui::Text(("Mass: " + std::format("{:.0f}kg", mass * 1000.f)).c_str());
			else ImGui::Text(("Mass: " + std::format("{:.2f} tonnes", mass)).c_str());

			ImGui::TreePop();
		}

		float distance = train.GetTargetDistanceRemaining();
		if (distance < 1000)
			ImGui::Text(("Distance to destination: " + std::format("{:.1f} m", distance)).c_str());
		else
			ImGui::Text(("Distance to destination: " + std::format("{:.2f} km", distance / 1000.f)).c_str());

		ImGui::Text(("Velocity: " + std::format("{:.1f}m/s", abs(train.GetVelocity()))).c_str());
		if (m_advancedInfo)
		{
			m_acceleration = std::lerp(m_acceleration, (train.GetVelocity() - m_lastFrameVelocity) / deltaTime, deltaTime * 5.f);
			ImGui::Text(("Acceleration: " + std::format("{:.2f}m/s2", m_acceleration)).c_str());
			m_lastFrameVelocity = train.GetVelocity();
			ImGui::Text(("Braking distance: " + std::format("{:.1f}m", abs(train.GetMaxStoppingDistance()))).c_str());
			if (train.GetBraking() && train.GetVelocity() != 0.f) ImGui::Text("State: Braking");
			else if (train.GetCurrentAcceleration() > 0)ImGui::Text("State: Accelerating >");
			else if (train.GetCurrentAcceleration() < 0)ImGui::Text("State: < Accelerating");
			else ImGui::Text("State: Idle");
		}

		ImGui::SeparatorEx(ImGuiSeparatorFlags_Horizontal);
		ImGui::Checkbox("Show Advanced info", &m_advancedInfo);
		ImGui::Checkbox("Show Braking distance indicator", &m_showBrakeDistance);

		Engine::UIManager::EndGameplayWindow();
		if (!open) m_selectedTrain = TrainID::Invalid, m_hasSelectedTrain = false;
	}
}
