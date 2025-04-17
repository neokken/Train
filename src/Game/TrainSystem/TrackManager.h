#pragma once
#include "TrackSegment.h"
#include "Helpers/IDGenerator.h"

#include <json.hpp>

class TrackManager
{
public:
	TrackManager() = default;

	TrackSegmentID BuildTrackPart( const float2& nodeA_Position, TrackDirection nodeA_direction, TrackSegmentID nodeA_connection, const float2& nodeB_Position, TrackDirection nodeB_direction, TrackSegmentID nodeB_connection );

	[[nodiscard]] bool IsValidTrackPart( const float2& nodeA_Position, TrackDirection nodeA_direction, TrackSegmentID nodeA_connection, const float2& nodeB_Position, TrackDirection nodeB_direction, TrackSegmentID nodeB_connection );

	[[nodiscard]] TrackNodeID GetNodeByPosition( const float2& position, float maxDifferance = .1f ) const;

	[[nodiscard]] bool DoesNodeExists( TrackNodeID id ) const;
	[[nodiscard]] bool DoesSegmentExists( TrackSegmentID id ) const;

	[[nodiscard]] const TrackNode& GetTrackNode( TrackNodeID id ) const;
	[[nodiscard]] const TrackSegment& GetTrackSegment( TrackSegmentID id ) const;

	[[nodiscard]] TrackSegmentID GetNextSegmentPositive( TrackSegmentID id ) const;
	[[nodiscard]] TrackSegmentID GetNextSegmentNegative( TrackSegmentID id ) const;

	// saving & loading data
	[[nodiscard]] nlohmann::json SerializeData() const;
	void LoadData( const nlohmann::json& data );

	[[nodiscard]] const std::unordered_map<TrackSegmentID, TrackSegment>& GetSegmentMap() const { return m_segments; }
	[[nodiscard]] const std::unordered_map<TrackNodeID, TrackNode>& GetNodeMap() const { return m_nodes; }

private:
	//nobody outside this class should touch nodes/segments
	// if there is a case handle it differently...

	TrackNodeID CreateNode( const float2& position );
	TrackSegmentID CreateSegment( TrackNodeID nodeA_ID, const float2& nodeA_direction, TrackNodeID nodeB_ID, const float2& nodeB_direction );

	void ConnectSegments( TrackSegmentID segmentA_ID, TrackSegmentID segmentB_ID, bool twoWayConnection = true );

	[[nodiscard]] TrackNode& GetMutableTrackNode( TrackNodeID id );
	[[nodiscard]] TrackSegment& GetMutableTrackSegment( TrackSegmentID id );

private:
	Engine::IDGenerator<TrackNodeID> m_nodeIDGenerator;
	Engine::IDGenerator<TrackSegmentID> m_segmentIDGenerator;

	std::unordered_map<TrackNodeID, TrackNode> m_nodes;
	std::unordered_map<TrackSegmentID, TrackSegment> m_segments;

	friend class TrackDebugger;
};
