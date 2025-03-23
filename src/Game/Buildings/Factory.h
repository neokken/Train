#pragma once

#include "Building.h"

namespace Game
{
	class Factory final : public Game::Building
	{
	public:
		Factory( const Engine::Transform& transform );

		void Start() override;
		void Update( float deltaTime ) override;
		void Render( const Engine::Camera& camera, Surface& target ) override;

		void ImGuiDebugViewer() override;

	private:
	};
}
