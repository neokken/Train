#pragma once

// TODO: Either pointerize or include these in any dependency with World.h
#include "Renderables/Grid.h"
#include "Camera/Camera.h"

namespace Engine {

	class GameObject;
	class InputManager;

	class World {
	public:
		World() = default;
		~World();

		// Life cycle functions
		void Init( Surface* renderTarget, InputManager* inputManager );
		void Update( float deltaTime );

		// World management functions
		void AddObject( GameObject* obj );
		void DestroyObject( GameObject* obj );

		const Camera& GetCamera() const;
		Surface* GetRenderTarget() const;
	private:
		std::vector<GameObject*> m_objects;

		Grid m_grid;
		Camera m_camera{ int2( 0,0 ) };

		Surface* m_renderTarget = nullptr;
	};
}