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
	ImGui::Text("Default view text if ImGuiDebugViewer has not been implemented for child of GameObject");
	ImGui::Text("If it is you! You are a lazy bum or something..");
}
