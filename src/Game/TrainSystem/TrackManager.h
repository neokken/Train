#pragma once
#include "TrackSegment.h"
#include "Helpers/IDGenerator.h"

#include <json.hpp>

class TrackManager
{
public:
	TrackManager() = default;

	TrackNodeID CreateNode( const float2& position );
	TrackSegmentID CreateSegment( TrackNodeID nodeA_ID, TrackNodeID nodeB_ID );

	void ConnectSegments( TrackSegmentID segmentA_ID, TrackSegmentID segmentB_ID, bool twoWayConnection = true );

	[[nodiscard]] bool DoesNodeExists( TrackNodeID id ) const;
	[[nodiscard]] bool DoesSegmentExists( TrackSegmentID id ) const;

	[[nodiscard]] const TrackNode& GetTrackNode( TrackNodeID id ) const;
	[[nodiscard]] const TrackSegment& GetTrackSegment( TrackSegmentID id ) const;

	[[nodiscard]] TrackSegmentID GetNextSegmentPositive( TrackSegmentID id ) const;
	[[nodiscard]] TrackSegmentID GetNextSegmentNegative( TrackSegmentID id ) const;

	// saving & loading data
	[[nodiscard]] nlohmann::json SerializeData() const;
	void LoadData( const nlohmann::json& data );

private:
	//nobody outside this class should touch nodes/segments
	// if there is a case handle it differently...
	[[nodiscard]] TrackNode& GetMutableTrackNode( TrackNodeID id );
	[[nodiscard]] TrackSegment& GetMutableTrackSegment( TrackSegmentID id );

private:
	Engine::IDGenerator<TrackNodeID> m_nodeIDGenerator;
	Engine::IDGenerator<TrackSegmentID> m_segmentIDGenerator;

	std::unordered_map<TrackNodeID, TrackNode> m_nodes;
	std::unordered_map<TrackSegmentID, TrackSegment> m_segments;

	friend class TrackDebugger;
};
