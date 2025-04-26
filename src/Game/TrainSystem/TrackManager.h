#pragma once
#include "TrackSegment.h"
#include "Helpers/IDGenerator.h"

#include <json.hpp>

struct AStarNode
{
	TrackSegmentID segment;
	float g = 0.f;
	float h = 0.f;
	[[nodiscard]] float f() const { return g + h; }
	const AStarNode* parent = nullptr;
	int parentLever = -1;
};

class TrackManager
{
public:
	TrackManager() = default;

	TrackSegmentID BuildTrackPart( const float2& nodeA_Position, TrackDirection nodeA_direction, TrackSegmentID nodeA_connection, const float2& nodeB_Position, TrackDirection nodeB_direction, TrackSegmentID nodeB_connection );

	void DeleteTrackPart( TrackSegmentID id );

	[[nodiscard]] bool IsValidTrackPart( const float2& nodeA_Position, TrackDirection nodeA_direction, TrackSegmentID nodeA_connection, const float2& nodeB_Position, TrackDirection nodeB_direction, TrackSegmentID nodeB_connection );

	[[nodiscard]] TrackNodeID GetNodeByPosition( const float2& position, float maxDifferance = .1f ) const;

	[[nodiscard]] bool DoesNodeExists( TrackNodeID id ) const;
	[[nodiscard]] bool DoesSegmentExists( TrackSegmentID id ) const;

	[[nodiscard]] const TrackNode& GetTrackNode( TrackNodeID id ) const;
	[[nodiscard]] const TrackSegment& GetTrackSegment( TrackSegmentID id ) const;
	//Get a track segment from its connecting nodes
	[[nodiscard]] const TrackSegmentID GetTrackSegment( TrackNodeID a, TrackNodeID b ) const;
	[[nodiscard]] const TrackSegmentID GetClosestTrackSegment( float2 position, float sqrMaxDistance = 50.f, bool returnFirstFound = false ) const;
	[[nodiscard]] float GetClosestDistanceOnTrackSegment( TrackSegmentID segmentID, float2 position ) const;

	[[nodiscard]] TrackSegmentID GetNextSegmentPositive( TrackSegmentID id ) const;
	[[nodiscard]] TrackSegmentID GetNextSegmentNegative( TrackSegmentID id ) const;

	// saving & loading data
	[[nodiscard]] nlohmann::json SerializeData() const;
	void LoadData( const nlohmann::json& data );

	[[nodiscard]] const std::unordered_map<TrackSegmentID, TrackSegment>& GetSegmentMap() const { return m_segments; }
	[[nodiscard]] const std::unordered_map<TrackNodeID, TrackNode>& GetNodeMap() const { return m_nodes; }

	std::vector<int> CalculatePath( const TrackSegmentID startID, bool startDirectionTowardsB, const TrackSegmentID targetID ) const;

	void SetNodeLever( const TrackNodeID node, TrackSegmentID segment, const int leverValue );

private:
	//nobody outside this class should touch nodes/segments
	// if there is a case handle it differently...

	TrackNodeID CreateNode( const float2& position );
	TrackSegmentID CreateSegment( TrackNodeID nodeA_ID, const float2& nodeA_direction, TrackNodeID nodeB_ID, const float2& nodeB_direction );

	void ConnectSegments( TrackSegmentID segmentA_ID, TrackSegmentID segmentB_ID, bool twoWayConnection = true );

	[[nodiscard]] TrackNode& GetMutableTrackNode( TrackNodeID id );
	[[nodiscard]] TrackSegment& GetMutableTrackSegment( TrackSegmentID id );

	[[nodiscard]] float SQRDistancePointToSegment( const float2& position, const TrackSegment& segment ) const;

	//A*
	//Estimated heuristic for a path ( h )
	[[nodiscard]] float PathHeuristic( TrackNodeID a, TrackNodeID b ) const;
	//Actual heuristic ( g )
	[[nodiscard]] static float PathDistance( const TrackSegment& segment );
	[[nodiscard]] static std::vector<int> ReconstructPath( const AStarNode& finalNode );

private:
	Engine::IDGenerator<TrackNodeID> m_nodeIDGenerator;
	Engine::IDGenerator<TrackSegmentID> m_segmentIDGenerator;

	std::unordered_map<TrackNodeID, TrackNode> m_nodes;
	std::unordered_map<TrackSegmentID, TrackSegment> m_segments;

	friend class TrackDebugger;
};
