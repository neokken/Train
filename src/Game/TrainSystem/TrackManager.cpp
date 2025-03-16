#include "precomp.h"
#include "TrackManager.h"

TrackNodeID TrackManager::CreateNode( const float2& position )
{
	const TrackNodeID newID = m_nodeIDGenerator.GenerateID();

	TrackNode newNode;
	newNode.m_id = newID;
	newNode.m_nodePosition = position;

	m_nodes[newID] = newNode;
	return newID;
}

TrackSegmentID TrackManager::CreateSegment( const TrackNodeID nodeA, const TrackNodeID nodeB )
{
	const TrackSegmentID newID = m_segmentIDGenerator.GenerateID();

	TrackSegment newSegment;
	newSegment.m_id = newID;
	newSegment.m_nodeA = nodeA;
	newSegment.m_nodeB = nodeB;

	m_segments[newID] = newSegment;

	GetTrackNode(nodeA).AddTrackSegment(newID);
	GetTrackNode(nodeB).AddTrackSegment(newID);

	return newID;
}

void TrackManager::ConnectSegments( const TrackSegmentID segmentA_ID, const TrackSegmentID segmentB_ID, const bool twoWayConnection )
{
	const TrackSegment& segmentA = GetTrackSegment(segmentA_ID);
	const TrackSegment& segmentB = GetTrackSegment(segmentB_ID);

	const TrackNodeID segmentA_nodeA = segmentA.m_nodeA;
	const TrackNodeID segmentA_nodeB = segmentA.m_nodeB;
	const TrackNodeID segmentB_nodeA = segmentB.m_nodeA;
	const TrackNodeID segmentB_nodeB = segmentB.m_nodeB;

	TrackNodeID connectedNodeID = TrackNodeID::Invalid;

	if (segmentA_nodeA == segmentB_nodeA || segmentA_nodeA == segmentB_nodeB)
	{
		connectedNodeID = segmentA_nodeA;
	}
	else if (segmentA_nodeB == segmentB_nodeA || segmentA_nodeB == segmentB_nodeB)
	{
		connectedNodeID = segmentA_nodeB;
	}

	if (connectedNodeID == TrackNodeID::Invalid)
	{
		Engine::Logger::Warn("TrackSegments don't share a common node.");
	}

	TrackNode& connectedNode = GetTrackNode(connectedNodeID);

	connectedNode.AddConnection(segmentA_ID, segmentB_ID);
	if (twoWayConnection)
	{
		connectedNode.AddConnection(segmentB_ID, segmentA_ID);
	}
}

const TrackNode& TrackManager::GetTrackNode( const TrackNodeID id ) const
{
	const auto it = m_nodes.find(id);
	if (it == m_nodes.end())
	{
		throw std::runtime_error("Invalid TrackNode ID");
	}
	return it->second;
}

const TrackSegment& TrackManager::GetTrackSegment( const TrackSegmentID id ) const
{
	const auto it = m_segments.find(id);
	if (it == m_segments.end())
	{
		throw std::runtime_error("Invalid TrackSegment ID");
	}

	return it->second;
}

TrackNode& TrackManager::GetTrackNode( const TrackNodeID id )
{
	const auto it = m_nodes.find(id);
	if (it == m_nodes.end())
	{
		throw std::runtime_error("Invalid TrackNode ID");
	}
	return it->second;
}

TrackSegment& TrackManager::GetTrackSegment( const TrackSegmentID id )
{
	const auto it = m_segments.find(id);
	if (it == m_segments.end())
	{
		throw std::runtime_error("Invalid TrackSegment ID");
	}
	return it->second;
}
