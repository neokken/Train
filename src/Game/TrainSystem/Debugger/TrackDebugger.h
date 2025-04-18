#pragma once
#include "Game/TrainSystem/TrackManager.h"

namespace Engine
{
	class InputManager;
	class World;
	class Camera;
}

// CONFIGURABLE

constexpr float NODE_DISPLAY_SIZE = 1.f;
constexpr float NODE_SELECTION_DIST = 1.5f;
constexpr float SEGMENT_SELECTION_DIST = 1.f;

constexpr float NODE_SELECTION_DIST_SQ = NODE_SELECTION_DIST * NODE_SELECTION_DIST;
constexpr float SEGMENT_SELECTION_DIST_SQ = SEGMENT_SELECTION_DIST * SEGMENT_SELECTION_DIST;

enum DEBUG_COLORS
{
	NODE_COLOR_DEFAULT = 0x6610F2,
	NODE_COLOR_HOVER = 0x1A8FE3,
	NODE_COLOR_SELECT = 0xF17105,
	NODE_COLOR_SELECT_HOVER = 0xE6C229,

	SEGMENT_COLOR_DEFAULT = 0x6610F2,
	SEGMENT_COLOR_HOVER = 0x1A8FE3,
	SEGMENT_COLOR_SELECT = 0xF17105,
	SEGMENT_COLOR_SELECT_HOVER = 0xE6C229,
	SEGMENT_COLOR_LINKED = 0x8FB339,
};

class TrackDebugger
{
public:
	TrackDebugger() = default;

	void Init( TrackManager* trackManager );

	void Update( const Engine::Camera& camera );

	void Render( const Engine::Camera& camera, Surface& targetSurface ) const;

	void UI() const;

	void RenderTrackSegment( const Engine::Camera& camera, Surface& targetSurface, TrackSegmentID trackID, int segmentCount, uint color ) const;

private:
	// helper functions
	[[nodiscard]] float SQRDistancePointToSegment( const float2& position, const TrackSegment& segment ) const;

	[[nodiscard]] std::vector<TrackSegmentID> CalculateLinkedTrackSegments( TrackSegmentID segmentID ) const;
	[[nodiscard]] std::vector<TrackSegmentID> CalculateLinkedTrackSegments( TrackNodeID nodeID ) const;

private:
	TrackManager* m_trackManager{nullptr};

	TrackNodeID m_hoveredTrackNode{TrackNodeID::Invalid};
	TrackNodeID m_selectedTrackNode{TrackNodeID::Invalid};

	TrackSegmentID m_hoveredTrackSegment{TrackSegmentID::Invalid};
	TrackSegmentID m_selectedTrackSegment{TrackSegmentID::Invalid};

	std::vector<TrackSegmentID> m_linkedTrackSegments{};
};
