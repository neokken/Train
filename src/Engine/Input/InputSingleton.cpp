#include "precomp.h"
#include "InputSingleton.h"
#include "InputManager.h"

namespace
{
	Engine::InputManager* instance = nullptr;
}

namespace Input
{
	Engine::InputManager& get()
	{
		if (!instance)
		{
			instance = new Engine::InputManager();
		}
		return *instance;
	}
}
