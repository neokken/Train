#include "precomp.h"
#include "Helpers.h"

ScopedTimer::~ScopedTimer()
{
	Engine::Logger::Info( "{} tooks {}ms.", name, t.elapsed() * 1000 );
}