#include "precomp.h"
#include "Renderer.h"

namespace Engine
{
	Renderer* Renderer::m_instance{nullptr};

	Renderer& Engine::Renderer::GetRenderer()
	{
		if (m_instance == nullptr)
		{
			m_instance = new Renderer();
		}
		return *m_instance;
	}

	Renderer::Renderer()
	{
	}
}
