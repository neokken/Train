#include "precomp.h"
#include "Renderables/Grid.h"
#include "Camera/Camera.h"
#include "World.h"

#include "GameObject/GameObject.h"

// TODO: This is only testing code, this shouldn't ultimately be in engine code
#include "Game/Buildings/Building.h"

Engine::World::~World()
{
	for (auto& obj : m_objects) {
		delete obj;
	}
}

void Engine::World::Init( Surface* renderTarget, InputManager* inputManager )
{
	m_camera.Init( inputManager );
	m_camera.SetResolution( int2( SCRWIDTH, SCRHEIGHT ) );
	m_camera.SetZoomLevel( 1.f );

	m_grid.AddGrid( 0x354f52, 25 /*, .7f*/ );
	m_grid.AddGrid( 0x52796f, 100 /*, .33f*/ );

	m_renderTarget = renderTarget;

	Game::Building* building = new Game::Building(
		Engine::Transform{
			.position = float2( 0.0f ),
			.scale = float2( 1.0f )
		}
	);

	AddObject( building );
}

void Engine::World::Update( float deltaTime )
{
	m_objects.erase(
		std::remove_if(
			m_objects.begin(),
			m_objects.end(),
			[]( GameObject* obj ) { return obj->MarkedForDestroy(); }
		),
		m_objects.end()
	);


	m_camera.Update( deltaTime );

	// Render part
	m_grid.Render( m_camera, *m_renderTarget );

	// TODO: Move up to the updating part, this is a temp solution to draw over the grid
	for (auto& obj : m_objects) {
		obj->Update( deltaTime );
	}
}

void Engine::World::AddObject( GameObject* obj )
{
	obj->Init( this );
	m_objects.push_back( obj );
}

void Engine::World::DestroyObject( GameObject* obj )
{
	obj->Destroy();
}

const Engine::Camera& Engine::World::GetCamera() const
{
	return m_camera;
}

Surface* Engine::World::GetRenderTarget() const
{
	return m_renderTarget;
}
