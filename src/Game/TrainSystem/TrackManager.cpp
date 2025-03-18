#include "precomp.h"
#include "TrackManager.h"
#include "Serialization/TrackSerializer.h"

using json = nlohmann::json;

TrackNodeID TrackManager::CreateNode( const float2& position )
{
	const TrackNodeID newID = m_nodeIDGenerator.GenerateID();

	TrackNode newNode;
	newNode.m_id = newID;
	newNode.m_nodePosition = position;

	m_nodes[newID] = newNode;
	return newID;
}

TrackSegmentID TrackManager::CreateSegment( const TrackNodeID nodeA_ID, const TrackNodeID nodeB_ID )
{
	const TrackSegmentID newID = m_segmentIDGenerator.GenerateID();

	TrackSegment newSegment;
	newSegment.m_id = newID;
	newSegment.m_nodeA = nodeA_ID;
	newSegment.m_nodeB = nodeB_ID;

	TrackNode& nodeA = GetMutableTrackNode(nodeA_ID);
	TrackNode& nodeB = GetMutableTrackNode(nodeB_ID);

	if (nodeA.m_connectionLever.contains(newID))
	{
		nodeA.m_connectionLever[newID] = -1;
		nodeA.m_validConnections[newID];
	}

	if (nodeB.m_connectionLever.contains(newID))
	{
		nodeB.m_connectionLever[newID] = -1;
		nodeB.m_validConnections[newID];
	}

	// TODO: later this is will be replaced with a curved distance calculations
	newSegment.m_distance = length(nodeA.m_nodePosition - nodeB.m_nodePosition);

	m_segments[newID] = newSegment;
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

	TrackNode& connectedNode = GetMutableTrackNode(connectedNodeID);

	connectedNode.AddConnection(segmentA_ID, segmentB_ID);
	if (twoWayConnection)
	{
		connectedNode.AddConnection(segmentB_ID, segmentA_ID);
	}
}

bool TrackManager::DoesNodeExists( const TrackNodeID id ) const
{
	if (id == TrackNodeID::Invalid) return false;
	return m_nodes.contains(id);
}

bool TrackManager::DoesSegmentExists( const TrackSegmentID id ) const
{
	if (id == TrackSegmentID::Invalid) return false;
	return m_segments.contains(id);
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

TrackSegmentID TrackManager::GetNextSegmentPositive( const TrackSegmentID id ) const
{
	const TrackSegment segment = GetTrackSegment(id);
	const TrackNode node = GetTrackNode(segment.m_nodeA);

	const int connectionDir = node.m_connectionLever.at(id);
	if (connectionDir == -1) return TrackSegmentID::Invalid;

	return node.m_validConnections.at(id).at(connectionDir);
}

TrackSegmentID TrackManager::GetNextSegmentNegative( TrackSegmentID id ) const
{
	const TrackSegment segment = GetTrackSegment(id);
	const TrackNode node = GetTrackNode(segment.m_nodeB);

	const int connectionDir = node.m_connectionLever.at(id);
	if (connectionDir == -1) return TrackSegmentID::Invalid;

	return node.m_validConnections.at(id).at(connectionDir);
}

nlohmann::json TrackManager::SerializeData() const
{
	nlohmann::json j;

	j["nodeIDNext"] = m_nodeIDGenerator.GetSerializableData();
	j["segmentIDNext"] = m_segmentIDGenerator.GetSerializableData();

	j["nodes"] = m_nodes;
	j["segments"] = m_segments;

	return j;
}

void TrackManager::LoadData( const nlohmann::json& data )
{
	m_nodeIDGenerator.SetSerializableData(data.at("nodeIDNext").get<uint>());
	m_segmentIDGenerator.SetSerializableData(data.at("segmentIDNext").get<uint>());

	m_nodes = data.at("nodes").get<std::unordered_map<TrackNodeID, TrackNode>>();
	m_segments = data.at("segments").get<std::unordered_map<TrackSegmentID, TrackSegment>>();
}

TrackNode& TrackManager::GetMutableTrackNode( const TrackNodeID id )
{
	const auto it = m_nodes.find(id);
	if (it == m_nodes.end())
	{
		throw std::runtime_error("Invalid TrackNode ID");
	}
	return it->second;
}

TrackSegment& TrackManager::GetMutableTrackSegment( const TrackSegmentID id )
{
	const auto it = m_segments.find(id);
	if (it == m_segments.end())
	{
		throw std::runtime_error("Invalid TrackSegment ID");
	}
	return it->second;
}
