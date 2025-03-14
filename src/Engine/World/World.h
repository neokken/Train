#pragma once

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

	private:
		std::vector<GameObject*> m_objects;

		Grid m_grid;
		Camera m_camera{ int2( 0,0 ) };

		Surface* m_renderTarget = nullptr;
	};
}