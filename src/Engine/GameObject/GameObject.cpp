#include "precomp.h"
#include "GameObject.h"

Engine::GameObject::GameObject( const Transform& transform )
{
	m_transform = transform;
}

void Engine::GameObject::Init( World* world )
{
	m_world = world;
}

void Engine::GameObject::Start()
{
}

void Engine::GameObject::Destroy()
{
	m_destroy = true;
}

bool Engine::GameObject::MarkedForDestroy() const
{
	return m_destroy;
}

void Engine::GameObject::ImGuiDebugViewer()
{
	ImGui::Text("Override <ImGuiDebugViewer> to display GameObject specific content.");
}
