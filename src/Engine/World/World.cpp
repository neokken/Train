#include "precomp.h"
#include "Renderables/Grid.h"
#include "Camera/Camera.h"
#include "World.h"

#include "GameObject/GameObject.h"

// TODO: This is only testing code, this shouldn't ultimately be in engine code
#include "Game/Trains/Wagon.h"
#include "Game/Buildings/Building.h"
#include "Game/Trains/Locomotive.h"
#include "Game/Trains/Train.h"
#include "Game/TrainSystem/TrainWalker/TrackWalker.h"
#include "UI/UIManager.h"

Engine::World::~World()
{
	for (auto obj : m_objects)
	{
		delete obj;
	}
}

void Engine::World::Init( Surface* renderTarget )
{
	m_camera.SetResolution(int2(SCRWIDTH, SCRHEIGHT));
	m_camera.SetZoomLevel(50.f);

	m_grid.AddGrid(GetColor(Color::BackGroundGrid1), 1 /*, .7f*/);
	m_grid.AddGrid(GetColor(Color::BackGroundGrid2), 10 /*, .33f*/);

	m_renderTarget = renderTarget;

	m_trackBuilder.Init(&m_trackManager, &m_trackRenderer);
	m_trackRenderer.Init(&m_trackManager);
	m_trackDebugger.Init(&m_trackManager, &m_trackRenderer);
	m_trainDebugger.Init(m_trackManager, m_trainManager, m_trackBuilder);


	Game::Building* building = new Game::Building(Engine::Transform{.position = float2(0.0f), .scale = float2(1.0f)});

	AddObject(building);

	//Set up a track with a train on it for debugging
	m_trackManager.BuildTrackPart(float2(0, 0), TrackDirection::S, TrackSegmentID::Invalid, float2(0, 50), TrackDirection::S, TrackSegmentID::Invalid);

}

void Engine::World::Update( float deltaTime )
{
	std::erase_if(m_objects, []( GameObject* obj ) { return obj->MarkedForDestroy(); });

	// Update pass
	m_camera.Update(deltaTime);
	for (auto& obj : m_objects)
	{
		obj->Update(deltaTime);
	}

	m_trackBuilder.Update(m_camera, deltaTime);

	m_particles.Update(deltaTime);

	//m_trackDebugger.Update(m_camera);
	m_trainDebugger.Update(m_camera);
	if (Input::get().IsKeyJustDown(GLFW_KEY_P))
	{
		m_trackDebugger.EnableSelectMode(!m_trackDebugger.GetSelectMode());
		m_trackDebugger.SetVisible(true);
	}

	m_trackDebugger.Update(m_camera);

	// Render pass
	m_grid.Render(m_camera);

	m_trackRenderer.Render(m_camera, *m_renderTarget);
	for (const auto& obj : m_objects)
	{
		obj->Render(m_camera);
	}
	m_particles.Render(m_camera, *m_renderTarget);
	//m_trackDebugger.Render(m_camera, *m_renderTarget);
	m_trackBuilder.Render(m_camera, *m_renderTarget);

	m_trackDebugger.Render(m_camera);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void Engine::World::UI( const float deltaTime )
{
	//m_trackDebugger.UI();
	m_trainDebugger.UI(deltaTime);


	if (Engine::UIManager::BeginDebugWindow("GameObjects Debugger"))
	{
		for (int i = 0; i < m_objects.size(); i++)
		{
			std::string name = std::to_string(i);
			if (ImGui::TreeNode(name.c_str()))
			{
				m_objects.at(i)->ImGuiDebugViewer();

				ImGui::TreePop();
			}
		}
	}
	Engine::UIManager::EndDebugWindow();
}

void Engine::World::ImGuiBar()
{
	if (ImGui::MenuItem("TrackDebugger")) m_trackDebugger.SetVisible(!m_trackDebugger.GetVisibility());
}

void Engine::World::AddObject( GameObject* obj )
{
	obj->Init(this);
	m_objects.push_back(obj);
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
