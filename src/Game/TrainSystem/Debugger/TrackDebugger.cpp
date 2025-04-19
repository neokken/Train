#include "precomp.h"
#include "TrackDebugger.h"

#include "Input/InputManager.h"
#include "UI/UIManager.h"
#include "Camera/Camera.h"
#include "Renderables/Circle.h"
#include "Renderables/CurvedSegment.h"
#include "Renderables/LineSegment.h"

void TrackDebugger::Init( TrackManager* trackManager, TrackRenderer* trackRenderer )
{
	m_trackManager = trackManager;
	m_trackRenderer = trackRenderer;
}

void TrackDebugger::Update( const Engine::Camera& camera )
{
	if (!m_visible) return;
}

void TrackDebugger::Render( const Engine::Camera& camera, Surface& targetSurface ) const
{
	if (!m_visible) return;
}

void TrackDebugger::UI()
{
	if (!m_visible) return;

	if (Engine::UIManager::BeginDebugWindow("TrackDebugger", &m_visible))
	{
		ImGui::Text("gadasdasd");
	}
	Engine::UIManager::EndDebugWindow();
}

void TrackDebugger::SetVisible( const bool value )
{
	m_visible = value;
}

bool TrackDebugger::GetVisibility() const
{
	return m_visible;
}
