#include "precomp.h"
#include "Helpers.h"

Engine::ScopedTimer::~ScopedTimer()
{
	Engine::Logger::Info("{} tooks {}ms.", name, t.elapsed() * 1000);
}

float Engine::SqrDistancePointToSegment( const float2& point, const float2& A, const float2& B )
{
	// thanks chatgpt for this code...

	const float2 AB = B - A;
	const float2 AP = point - A;

	const float AB_lengthSq = sqrLength(AB);
	if (AB_lengthSq == 0.0f)
	{
		// A and B are the same point
		return sqrLength(AP);
	}

	// Project point onto the line (parametric t)
	float t = dot(AP, AB) / AB_lengthSq;
	t = std::clamp(t, 0.0f, 1.0f); // Clamping t to stay within segment

	// Find the closest point on the segment
	const float2 closestPoint = {A.x + t * AB.x, A.y + t * AB.y};

	// Compute distance from point to the closest point
	return sqrLength(point - closestPoint);
}
