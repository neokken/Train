#include "precomp.h"
#include "Renderables/Grid.h"
#include "Camera/Camera.h"
#include "World.h"

#include "GameObject/GameObject.h"

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

	for (auto& obj : m_objects) {
		obj->Update( deltaTime );
	}

	m_camera.Update( deltaTime );
	m_grid.Render( m_camera, *m_renderTarget );
}

void Engine::World::AddObject( GameObject* obj )
{
	m_objects.push_back( obj );
}

void Engine::World::DestroyObject( GameObject* obj )
{
	obj->Destroy();
}
