#pragma once

#include "GameObject/GameObject.h"
#include "Game/TrainSystem/TrackSegment.h"

namespace Game
{
	class Building : public Engine::GameObject
	{
	public:
		Building( Engine::Transform transform );
		void Update( float deltaTime ) override;
		void Render( const Engine::Camera& camera, Surface& target ) override;

		void ImGuiDebugViewer() override;

	protected:
		void RegisterNode( float2 position );
		std::vector<TrackNodeID> m_associatedNodes;
	};
}
