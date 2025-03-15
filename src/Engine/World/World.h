#pragma once

// TODO: Either pointerize or include these in any dependency with World.h
#include "Renderables/Grid.h"
#include "Camera/Camera.h"
#include "Game/TrainSystem/TrackManager.h"
#include "Game/TrainSystem/Debugger/TrackDebugger.h"

namespace Engine
{
	class GameObject;
	class InputManager;

	class World
	{
	public:
		World() = default;
		~World();

		// Life cycle functions
		void Init( Surface* renderTarget, InputManager* inputManager );
		void Update( float deltaTime );
		void UI();

		// World management functions
		void AddObject( GameObject* obj );
		void DestroyObject( GameObject* obj );

		const Camera& GetCamera() const;
		Surface* GetRenderTarget() const;

	private:
		std::vector<GameObject*> m_objects;

		Grid m_grid;
		Camera m_camera{int2(0, 0)};

		TrackManager m_trackManager;
		TrackDebugger m_trackDebugger;

		Surface* m_renderTarget = nullptr;
	};
}
