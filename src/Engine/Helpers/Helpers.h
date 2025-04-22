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

	float3 RGB8ToRGB32( uint color );

}

#define PROFILE_FUNCTION() Engine::ScopedTimer timer(__FUNCTION__)
