#pragma once

namespace Engine
{
	bool IsMouseOverUI();

	struct ScopedTimer
	{
		Timer t;
		const char* name;

		ScopedTimer( const char* funcName )
		{
			name = funcName;
		}

		~ScopedTimer();
	};
}

#define PROFILE_FUNCTION() Engine::ScopedTimer timer(__FUNCTION__)
