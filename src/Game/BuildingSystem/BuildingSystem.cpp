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

	constexpr float gridSize = 100.0f;

	worldPosMouse.x = roundf(worldPosMouse.x / gridSize) * gridSize;
	worldPosMouse.y = roundf(worldPosMouse.y / gridSize) * gridSize;

	Engine::Circle::RenderWorldPos(camera, renderTarget, worldPosMouse, 10.0f, 0xffffff);

	// TODO: When clicking on an already existing node, give the choice to select to which segment of that node to connect to

	if (m_inputManager->IsMouseJustDown(0))
	{
		TrackNodeID currentID = m_trackManager->CreateNode(worldPosMouse);

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
