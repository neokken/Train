#pragma once
#include "Game/TrainSystem/TrackSegment.h"

class TrackManager;

class TrackWalker
{
public:
	TrackWalker() = default;
	void Init( const TrackManager* tm );

	// returns true if moved the full
	bool Move( float distance );

	[[nodiscard]] const float& GetDistance() const { return m_distance; }

	[[nodiscard]] float2 GetPosition() const;
	[[nodiscard]] float2 GetDirection() const;

	[[nodiscard]] bool IsValid() const;

	[[nodiscard]] TrackSegmentID GetCurrentTrackSegment() const { return m_currentSegmentID; }
	void SetCurrentTrackSegment( TrackSegmentID newSegmentID, float newDistance );

	void ImGuiDebugViewer();

	/**
	 * Get the direction the walker is facing on the track
	 * @return true for pointing towards node B false for pointing to node A
	 */
	[[nodiscard]] bool GetTrackDirection() const { return !m_flipMoveDir; }

private:
	const TrackManager* m_trackManager{nullptr};

	TrackSegmentID m_currentSegmentID{TrackSegmentID::Invalid};

	float m_distance{0.f};
	bool m_flipMoveDir{false};
};
