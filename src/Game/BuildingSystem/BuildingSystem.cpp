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
	TrackSegmentID hoveredSegment = TrackSegmentID::Invalid;

	if (m_selectingSegments)
	{
		const TrackNode& node = static_cast<const TrackManager*>(m_trackManager)->GetTrackNode(m_selectingNodeID);

		bool anyHovered = false;
		for (auto [segmentID, segmentConnections] : node.m_validConnections)
		{
			bool hovered = false;
			const TrackSegment& segment = static_cast<const TrackManager*>(m_trackManager)->GetTrackSegment(segmentID);
			const TrackNode& a = static_cast<const TrackManager*>(m_trackManager)->GetTrackNode(segment.m_nodeA);
			const TrackNode& b = static_cast<const TrackManager*>(m_trackManager)->GetTrackNode(segment.m_nodeB);

			if (Engine::SqrDistancePointToSegment(worldPosMouse, a.m_nodePosition, b.m_nodePosition) < 100.0f)
			{
				if (!anyHovered)
				{
					anyHovered = true;
					hovered = true;
					hoveredSegment = segment.m_id;
				}
			}
			Engine::LineSegment::RenderWorldPos(camera, renderTarget, a.m_nodePosition, b.m_nodePosition, hovered ? 0xffff00 : 0xff0000);
		}
	}
	constexpr float gridSize = 100.0f;

	worldPosMouse.x = roundf(worldPosMouse.x / gridSize) * gridSize;
	worldPosMouse.y = roundf(worldPosMouse.y / gridSize) * gridSize;

	Engine::Circle::RenderWorldPos(camera, renderTarget, worldPosMouse, 10.0f, 0xffffff);

	if (m_inputManager->IsMouseJustDown(0))
	{
		if (m_selectingSegments)
		{
			if (hoveredSegment == TrackSegmentID::Invalid) return;
			m_selectingSegments = false;
			m_lastSegmentID = hoveredSegment;
			printf("Selected segment.\n");
			return;
		}

		TrackNodeID currentID = m_trackManager->GetTrackNodeAtPosition(worldPosMouse);
		if (currentID == TrackNodeID::Invalid)
		{
			currentID = m_trackManager->CreateNode(worldPosMouse);
		}
		else
		{
			if (TrackSegmentID id = TryAutoSelectSegment(currentID); id != TrackSegmentID::Invalid)
			{
				m_lastSegmentID = id;
			}
			else
			{
				m_selectingNodeID = currentID;
				m_selectingSegments = true;
				m_selectedTrackSegment = TrackSegmentID::Invalid;
				return;
			}
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

TrackSegmentID Game::BuildingSystem::TryAutoSelectSegment( const TrackNodeID& id ) const
{
	if (id == TrackNodeID::Invalid) return TrackSegmentID::Invalid;
	const TrackNode& node = static_cast<const TrackManager*>(m_trackManager)->GetTrackNode(id);

	if (node.m_validConnections.size() == 1)
	{
		auto it = node.m_validConnections.begin();
		return it->first;
	}

	return TrackSegmentID::Invalid;
}
