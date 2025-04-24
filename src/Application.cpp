// Template, 2024 IGAD Edition
// Get the latest version from: https://github.com/jbikker/tmpl8
// IGAD/NHTV/BUAS/UU - Jacco Bikker - 2006-2024

#include "precomp.h"
#include "Application.h"

#include "Renderables/Circle.h"
#include "Renderables/LineSegment.h"
#include "UI/UIManager.h"

int startID, endID;
bool towardsB;
std::vector<int> path;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Application::Init()
{
	Engine::Logger::Init();
	Engine::UIManager::Init();
	m_world.Init(screen);
}

// -----------------------------------------------------------
// Main application tick function - Executed once per frame
// -----------------------------------------------------------
void Application::Tick( const float deltaTime )
{
	m_frameTimer.reset();
	screen->Clear(GetColor(Color::BackGround));

	// Update logic
	m_world.Update(deltaTime);
	//DEBUG BEHAVIOUR
	if (Input::get().IsKeyJustDown(GLFW_KEY_F1))
	{
		Engine::UIManagerSettings settings = Engine::UIManager::GetSettings();
		settings.debugMode = !settings.debugMode;
		Engine::UIManager::SetSettings(settings);
	}

	if (!path.empty())
	{
		TrackManager& tm = m_world.GetTrackManager();
		TrackSegmentID seg = (TrackSegmentID)startID;
		TrackNodeID node = towardsB ? tm.GetTrackSegment(seg).nodeB : tm.GetTrackSegment(seg).nodeA;
		for (int i = 0; i < path.size(); ++i)
		{
			const TrackSegment& nod = tm.GetTrackSegment(seg);
			Engine::Circle::RenderWorldPos(m_world.GetCamera(), *screen, nod.nodeA_Position, 0.6f, 0xff00);
			Engine::Circle::RenderWorldPos(m_world.GetCamera(), *screen, nod.nodeB_Position, 0.5f, 0xff0000);
			Engine::LineSegment::RenderWorldPos(m_world.GetCamera(), *screen, nod.nodeA_Position, nod.nodeB_Position, 0xff0000);
			if (path[i] == -1) break;
			seg = tm.GetTrackNode(node).validConnections.at(seg)[path[i]];
			const TrackSegment& nextSeg = tm.GetTrackSegment(seg);
			if (nextSeg.nodeA == node) node = nextSeg.nodeB;
			else node = nextSeg.nodeA;
		}
	}
	



	Input::get().Update(deltaTime);
}

void Tmpl8::Application::UI()
{
	if (Engine::UIManager::BeginDebugWindow("pathfinder"))
	{
		ImGui::InputInt("StartSeg ID", &startID);
		ImGui::Checkbox("Towards B", &towardsB);
		ImGui::InputInt("EndSeg ID", &endID);
		if (ImGui::Button("Calculate path"))
		{
			path = m_world.GetTrackManager().CalculatePath((TrackSegmentID)startID, towardsB, (TrackSegmentID)endID);
		}
	}
	Engine::UIManager::EndDebugWindow();

	if (Engine::UIManager::BeginDebugWindow("Window"))
	{
		ImGui::Text("Frametime: %fms", m_frameTimer.elapsed() * 1000);
	}
	Engine::UIManager::EndDebugWindow();

	m_world.UI();
}
