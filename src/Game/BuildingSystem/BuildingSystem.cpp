#include "precomp.h"
#include "BuildingSystem.h"

#include "Camera/Camera.h"
#include "Input/InputManager.h"
#include "Game/TrainSystem/TrackManager.h"
#include "Renderables/Circle.h"
#include "Renderables/LineSegment.h"
#include "UI/UIManager.h"

void Game::BuildingSystem::Init( Engine::InputManager* inputManager, TrackManager* trackManager )
{
	m_inputManager = inputManager;
	m_trackManager = trackManager;
}

void Game::BuildingSystem::Update( float /*deltaTime*/ )
{
	// TODO: To be moved somewhere else
	if (m_inputManager->IsKeyJustDown(GLFW_KEY_L))
	{
		SetActive(!IsActive());
	}

	if (!IsActive()) return;
}

void Game::BuildingSystem::Render( const Engine::Camera& camera, Surface& renderTarget )
{
	if (!IsActive()) return;
	float2 worldPosMouse = camera.GetWorldPosition(m_inputManager->GetMousePos());

	if (m_selectingSegments)
	{
		const TrackNode& node = static_cast<const TrackManager*>(m_trackManager)->GetTrackNode(m_lastNodeID);
		for (auto [segmentID, segmentConnections] : node.m_validConnections)
		{
			const TrackSegment& segment = static_cast<const TrackManager*>(m_trackManager)->GetTrackSegment(segmentID);
			const TrackNode& a = static_cast<const TrackManager*>(m_trackManager)->GetTrackNode(segment.m_nodeA);
			const TrackNode& b = static_cast<const TrackManager*>(m_trackManager)->GetTrackNode(segment.m_nodeB);

			bool hovered = false;

			if (Engine::SqrDistancePointToSegment(worldPosMouse, a.m_nodePosition, b.m_nodePosition) < 100.0f)
			{
				hovered = true;
				if (m_inputManager->IsMouseJustDown(GLFW_MOUSE_BUTTON_LEFT))
				{
					if (m_selectedTrackSegment != segment.m_id)
					{
						m_selectedTrackSegment = segment.m_id;
					}
					else
					{
						m_selectedTrackSegment = TrackSegmentID::Invalid;
					}
				}

				Engine::LineSegment::RenderWorldPos(camera, renderTarget, a.m_nodePosition, b.m_nodePosition, hovered ? 0xffff00 : 0xff0000);
				return;
			}
		}

		if (m_selectedTrackSegment != TrackSegmentID::Invalid)
		{
			m_selectingSegments = false;
			m_lastSegmentID = m_selectedTrackSegment;
		}

		return;
	}
	constexpr float gridSize = 100.0f;

	worldPosMouse.x = roundf(worldPosMouse.x / gridSize) * gridSize;
	worldPosMouse.y = roundf(worldPosMouse.y / gridSize) * gridSize;

	Engine::Circle::RenderWorldPos(camera, renderTarget, worldPosMouse, 10.0f, 0xffffff);

	if (m_inputManager->IsMouseJustDown(0))
	{
		TrackNodeID currentID = m_trackManager->GetTrackNodeAtPosition(worldPosMouse);
		if (currentID == TrackNodeID::Invalid)
		{
			currentID = m_trackManager->CreateNode(worldPosMouse);
		}
		else
		{
			m_lastNodeID = currentID;
			m_selectingSegments = true;
			m_selectedTrackSegment = TrackSegmentID::Invalid;
			return;
		}

		if (m_lastNodeID != TrackNodeID::Invalid)
		{
			TrackSegmentID segmentID = m_trackManager->CreateSegment(m_lastNodeID, currentID);

			if (m_lastSegmentID != TrackSegmentID::Invalid)
			{
				m_trackManager->ConnectSegments(m_lastSegmentID, segmentID);
			}

			m_lastSegmentID = segmentID;
		}

		m_lastNodeID = currentID;
	}
}

void Game::BuildingSystem::SetActive( bool active )
{
	if (active == false)
	{
		m_lastSegmentID = TrackSegmentID::Invalid;
		m_lastNodeID = TrackNodeID::Invalid;
	}
	m_active = active;
}
