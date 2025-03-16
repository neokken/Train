#pragma once

#include "Game/TrainSystem/TrackSegment.h"

namespace Engine
{
	class Camera;
	class InputManager;
}

class TrackManager;

namespace Game
{
	class BuildingSystem
	{
	public:
		BuildingSystem() = default;
		void Init( Engine::InputManager* inputManager, TrackManager* trackManager );

		void Update( float deltaTime );
		void Render( const Engine::Camera& camera, Surface& renderTarget );

		bool IsActive() const { return m_active; }
		void SetActive( bool active );

	private:
		float2 m_nodePreviewPos;

		Engine::InputManager* m_inputManager = nullptr;
		TrackManager* m_trackManager = nullptr;

		TrackNodeID m_lastNodeID;
		TrackSegmentID m_lastSegmentID;

		bool m_active = false;
	};
}
