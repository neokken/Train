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
		Engine::InputManager* m_inputManager = nullptr;
		TrackManager* m_trackManager = nullptr;

		TrackNodeID m_lastNodeID;
		TrackNodeID m_selectingNodeID;
		TrackSegmentID m_lastSegmentID;

		bool m_selectingSegments = false;
		TrackSegmentID m_selectedTrackSegment;
		bool m_active = false;

		TrackSegmentID TryAutoSelectSegment( const TrackNodeID& id ) const;

		// 0 building rails
		// 1 creating junctions
		uint m_mode = 0;
	};
}
