#pragma once
#include "TrackSegment.h"
#include "IDGenerator.h"

class TrackManager
{
public:
	TrackManager() = default;

	TrackNodeID CreateNode(const float2& position);
	TrackSegmentID CreateSegment(TrackNodeID nodeA, TrackNodeID nodeB);

	void ConnectSegments(TrackSegmentID segmentA_ID, TrackSegmentID segmentB_ID, bool twoWayConnection=true);


	[[nodiscard]] const TrackNode& GetTrackNode(TrackNodeID id) const;
	[[nodiscard]] const TrackSegment& GetTrackSegment(TrackSegmentID id) const;

private:
	//nobody outside this class should touch nodes/segments
	// if there is a case handle it differently...
	[[nodiscard]] TrackNode& GetTrackNode(TrackNodeID id);
	[[nodiscard]] TrackSegment& GetTrackSegment(TrackSegmentID id);

private:
	IDGenerator<TrackNodeID> m_nodeIDGenerator;
	IDGenerator<TrackSegmentID> m_segmentIDGenerator;

	std::unordered_map<TrackNodeID, TrackNode> m_nodes;
	std::unordered_map<TrackSegmentID, TrackSegment> m_segments;


	friend class TrackDebugger;
};

