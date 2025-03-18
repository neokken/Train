#pragma once
#include "Game/TrainSystem/TrackSegment.h"

class TrackManager;

class TrainWalker
{
public:
	TrainWalker() = default;
	void Init( const TrackManager* tm );

	// returns true if moved the full m_distance
	bool Move( float distance );

	[[nodiscard]] const float& GetDistance() const { return m_distance; }

	[[nodiscard]] float2 GetPosition() const;

	[[nodiscard]] bool IsValid() const;

private:
	const TrackManager* m_trackManager{nullptr};

	TrackSegmentID m_currentSegmentID{TrackSegmentID::Invalid};

	float m_distance{0.f};
};
