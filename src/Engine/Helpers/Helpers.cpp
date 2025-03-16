#include "precomp.h"
#include "Helpers.h"

Engine::ScopedTimer::~ScopedTimer()
{
	Engine::Logger::Info("{} tooks {}ms.", name, t.elapsed() * 1000);
}
