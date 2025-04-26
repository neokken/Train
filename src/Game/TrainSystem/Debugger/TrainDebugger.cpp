#include "precomp.h"
#include "TrainDebugger.h"

#include "Camera/Camera.h"
#include "Game/Trains/Train.h"
#include "Game/Trains/Wagon.h"
#include "Game/TrainSystem/TrackBuilder.h"
#include "Game/TrainSystem/TrackManager.h"
#include "Renderables/Circle.h"
#include "Renderables/CurvedSegment.h"
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

	auto trainMap = m_trainManager->GetTrainMap();
	for (auto train : trainMap)
	{
		for (auto wagon : train.second->GetWagons())
		{
			if (length(wagon->GetTransform().position - worldPosMouse) < 3.f)
			{
				Engine::Circle::RenderWorldPos(camera, wagon->GetTransform().position, 2.f, 0x3030ff);
				if (input.IsMouseClicked(0))
				{
					m_selectedTrain = train.first;
				}
			}
		}
	}

	//Train manual control
	if (m_selectedTrain != TrainID::Invalid
		&& !m_trackBuilder->InBuildMode()
		&& input.IsKeyDown(GLFW_KEY_LEFT_CONTROL))
	{
		m_targetSegment = m_trackManager->GetClosestTrackSegment(worldPosMouse);
		if (m_targetSegment != TrackSegmentID::Invalid)
		{
			m_targetDistance = m_trackManager->GetClosestDistanceOnTrackSegment(m_targetSegment, worldPosMouse);
			const TrackSegment& segment = m_trackManager->GetTrackSegment(m_targetSegment);
			Engine::CurveData curve = {segment.nodeA_Position, segment.nodeA_Direction, segment.nodeB_Position, segment.nodeB_Direction};
			Engine::Circle::RenderWorldPos(camera, Engine::CurvedSegment::GetPositionOnCurvedSegment(m_targetDistance, curve), 1.f, 0xff00ff);

			//Show path
			TrackSegmentID currentSegment = m_trainManager->GetTrain(m_selectedTrain).GetWagons()[0]->GetFrontWalker().GetCurrentTrackSegment();
			std::vector<int> path = m_trackManager->CalculatePath(currentSegment, true, m_targetSegment);
			TrackNodeID currentNode = m_trackManager->GetTrackSegment(currentSegment).nodeB;

			TrackSegment currentSeg = m_trackManager->GetTrackSegment(currentSegment);
			curve = {currentSeg.nodeA_Position, currentSeg.nodeA_Direction, currentSeg.nodeB_Position, currentSeg.nodeB_Direction};
			curve.baseSegments = static_cast<uint>(currentSeg.distance * 2.f);
			float2 range = float2(m_trainManager->GetTrain(m_selectedTrain).GetWagons()[0]->GetFrontWalker().GetDistance() / currentSeg.distance, 1.f);
			if (path.empty()) range.y = m_targetDistance;
			Engine::CurvedSegment::RenderArrowsWorldPos(camera, curve, 0xff0000, 0.7f, range);

			for (int i = 0; i < static_cast<int>(path.size()); ++i)
			{
				currentSegment = m_trackManager->GetTrackNode(currentNode).validConnections.at(currentSegment)[path[i]];
				currentSeg = m_trackManager->GetTrackSegment(currentSegment);
				if (currentSeg.nodeA == currentNode)
				{
					currentNode = currentSeg.nodeB;
					curve = {currentSeg.nodeA_Position, currentSeg.nodeA_Direction, currentSeg.nodeB_Position, currentSeg.nodeB_Direction};
					curve.baseSegments = static_cast<uint>(currentSeg.distance * 2.f);
					float2 range = (i == static_cast<int>(path.size()) - 1) ? float2(0.f, m_targetDistance) : float2(0.f, 1.f);
					Engine::CurvedSegment::RenderArrowsWorldPos(camera, curve, 0xff0000, 0.7f, range);
				}
				else
				{
					currentNode = currentSeg.nodeA;
					currentNode = currentSeg.nodeB;
					curve = {currentSeg.nodeB_Position, currentSeg.nodeB_Direction, currentSeg.nodeA_Position, currentSeg.nodeA_Direction};
					curve.baseSegments = static_cast<uint>(currentSeg.distance * 2.f);
					float2 range = (i == static_cast<int>(path.size()) - 1) ? float2(0.f, 1.f - m_targetDistance) : float2(0.f, 1.f);
					Engine::CurvedSegment::RenderArrowsWorldPos(camera, curve, 0xff0000, 0.7f, range);
				}
			}

			if (input.IsMouseClicked(0))
			{
				m_trainManager->SetTrainDestination(m_selectedTrain, m_targetSegment, m_targetDistance);
			}
		}
	}



}

void TrainDebugger::UI()
{
	if (m_selectedTrain != TrainID::Invalid)
	{
		bool open = true;
		Engine::UIManager::BeginGameplayWindow(("Train: " + std::to_string(static_cast<int>(m_selectedTrain))).c_str(), open);
		ImGui::Text("Train");
		Engine::UIManager::EndGameplayWindow();
		if (!open) m_selectedTrain = TrainID::Invalid;
	}
}
