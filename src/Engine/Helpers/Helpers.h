#pragma once

namespace Engine
{
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

	float SqrDistancePointToSegment( const float2& point, const float2& A, const float2& B );
}

#define PROFILE_FUNCTION() Engine::ScopedTimer timer(__FUNCTION__)
