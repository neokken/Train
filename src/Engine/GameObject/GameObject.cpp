#include "precomp.h"
#include "GameObject.h"

void Engine::GameObject::Destroy()
{
	m_destroy = true;
}

bool Engine::GameObject::MarkedForDestroy() const
{
	return m_destroy;
}