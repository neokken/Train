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

	// TODO: Implement mouse reading
}

void Game::BuildingSystem::Render( const Engine::Camera& camera, Surface& renderTarget )
{
	if (!IsActive()) return;
	float2 worldPosMouse = camera.GetWorldPosition(m_inputManager->GetMousePos());

	constexpr float gridSize = 100.0f;

	worldPosMouse.x = roundf(worldPosMouse.x / gridSize) * gridSize;
	worldPosMouse.y = roundf(worldPosMouse.y / gridSize) * gridSize;

	Engine::Circle::RenderWorldPos(camera, renderTarget, worldPosMouse, 10.0f, 0xffffff);

	if (m_inputManager->IsMouseJustDown(0))
	{
		uint32_t currentID = static_cast<uint32_t>(m_trackManager->CreateNode(worldPosMouse));

		if (static_cast<TrackNodeID>(m_lastNodeID) != TrackNodeID::Invalid)
		{
			m_trackManager->CreateSegment((TrackNodeID)m_lastNodeID, (TrackNodeID)currentID);
		}

		m_lastNodeID = currentID;

		// TODO: Connect segments
	}
}

void Game::BuildingSystem::SetActive( bool active )
{
	m_active = active;
}
