#include "precomp.h"
#include "Renderables/Grid.h"
#include "Camera/Camera.h"
#include "World.h"

#include "GameObject/GameObject.h"

// TODO: This is only testing code, this shouldn't ultimately be in engine code
#include "Wagon.h"
#include "Game/Buildings/Building.h"
#include "Game/TrainSystem/Partials/TrackWalkerVisualizer.h"
#include "Game/TrainSystem/TrainWalker/TrackWalker.h"
#include "Renderables/CurvedSegment.h"
#include "UI/UIManager.h"

Engine::World::~World()
{
	for (auto obj : m_objects)
	{
		delete obj;
	}
}

void Engine::World::Init( Surface* renderTarget, InputManager* inputManager )
{
	m_camera.Init(inputManager);
	m_camera.SetResolution(int2(SCRWIDTH, SCRHEIGHT));
	m_camera.SetZoomLevel(50.f);

	m_grid.AddGrid(GetColor(Color::BackGroundGrid1), 1 /*, .7f*/);
	m_grid.AddGrid(GetColor(Color::BackGroundGrid2), 10 /*, .33f*/);

	m_renderTarget = renderTarget;

	m_trackBuilder.Init(inputManager, &m_trackManager, &m_trackRenderer);
	m_trackRenderer.Init(&m_trackManager);
	m_trackDebugger.Init(inputManager, &m_trackManager);

	Game::Building* building = new Game::Building(Engine::Transform{.position = float2(0.0f), .scale = float2(1.0f)});

	AddObject(building);

	TrackSegmentID seg1 = m_trackManager.BuildTrackPart(float2(0, 0), TrackDirection::S, TrackSegmentID::Invalid, float2(0, 20), TrackDirection::S, TrackSegmentID::Invalid);

	TrackWalker tWalk;
	tWalk.Init(&m_trackManager);
	tWalk.SetCurrentTrackSegment(seg1, 0);
	AddObject(new Wagon(tWalk));

	// Rails
	/*
	 *	This forms branch line, where a secondary track splits of the main on.
	 *	This shows that the direction you can go depends on where you come from.
	 *	going north from the mainline, you can only keep going north.
	 *	going north up from the branch line, you can only keep going north merging on the main line.
	 *	going south there is a split where you can choose
	 */
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

	m_trackDebugger.Update(m_camera);
	m_trackBuilder.Update(m_camera, deltaTime);

	// Render pass
	m_grid.Render(m_camera, *m_renderTarget);

	m_trackRenderer.Render(m_camera, *m_renderTarget);
	for (const auto& obj : m_objects)
	{
		obj->Render(m_camera, *m_renderTarget);
	}
	//m_trackDebugger.Render(m_camera, *m_renderTarget);
	m_trackBuilder.Render(m_camera, *m_renderTarget);
}

// ReSharper disable once CppMemberFunctionMayBeConst
void Engine::World::UI()
{
	m_trackDebugger.UI();

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
