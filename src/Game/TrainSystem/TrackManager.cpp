#include "precomp.h"
#include "TrackManager.h"
#include "Serialization/TrackSerializer.h"

using json = nlohmann::json;

TrackNodeID TrackManager::CreateNode( const float2& position )
{
	const TrackNodeID newID = m_nodeIDGenerator.GenerateID();

	TrackNode newNode;
	newNode.id = newID;
	newNode.nodePosition = position;

	m_nodes[newID] = newNode;
	return newID;
}

TrackSegmentID TrackManager::CreateSegment( TrackNodeID nodeA_ID, const float2& nodeA_direction, TrackNodeID nodeB_ID, const float2& nodeB_direction )
{
	const TrackSegmentID newID = m_segmentIDGenerator.GenerateID();

	TrackSegment newSegment;
	newSegment.id = newID;
	newSegment.nodeA = nodeA_ID;
	newSegment.nodeB = nodeB_ID;
	newSegment.nodeA_Direction = normalize(nodeA_direction);
	newSegment.nodeB_Direction = normalize(nodeB_direction);

	TrackNode& nodeA = GetMutableTrackNode(nodeA_ID);
	TrackNode& nodeB = GetMutableTrackNode(nodeB_ID);

	if (!nodeA.connectionLever.contains(newID))
	{
		nodeA.connectionLever[newID] = -1;
		nodeA.validConnections[newID];
	}

	if (!nodeB.connectionLever.contains(newID))
	{
		nodeB.connectionLever[newID] = -1;
		nodeB.validConnections[newID];
	}

	// TODO: later this is will be replaced with a curved distance calculations
	newSegment.distance = length(nodeA.nodePosition - nodeB.nodePosition);

	m_segments[newID] = newSegment;
	return newID;
}

void TrackManager::ConnectSegments( const TrackSegmentID segmentA_ID, const TrackSegmentID segmentB_ID, const bool twoWayConnection )
{
	const TrackSegment& segmentA = GetTrackSegment(segmentA_ID);
	const TrackSegment& segmentB = GetTrackSegment(segmentB_ID);

	const TrackNodeID segmentA_nodeA = segmentA.nodeA;
	const TrackNodeID segmentA_nodeB = segmentA.nodeB;
	const TrackNodeID segmentB_nodeA = segmentB.nodeA;
	const TrackNodeID segmentB_nodeB = segmentB.nodeB;

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
		Engine::Logger::Warn("TrackSegments {} and {}, don't share a common node.", static_cast<int>(segmentA_ID), static_cast<int>(segmentB_ID));
		return;
	}

	TrackNode& connectedNode = GetMutableTrackNode(connectedNodeID);

	connectedNode.AddConnection(segmentA_ID, segmentB_ID);
	if (twoWayConnection)
	{
		connectedNode.AddConnection(segmentB_ID, segmentA_ID);
	}
}

TrackSegmentID TrackManager::BuildTrackPart( const float2& nodeA_Position, const TrackDirection nodeA_direction, const TrackSegmentID nodeA_connection, const float2& nodeB_Position, const TrackDirection nodeB_direction, const TrackSegmentID nodeB_connection )
{
	if (!IsValidTrackPart(nodeA_Position, nodeA_direction, nodeA_connection, nodeB_Position, nodeB_direction, nodeB_connection))
	{
		Engine::Logger::Warn("Could not place track part, since its not a valid connection.");
		return TrackSegmentID::Invalid;
	}

	TrackNodeID nodeA_ID = GetNodeByPosition(nodeA_Position);
	TrackNodeID nodeB_ID = GetNodeByPosition(nodeB_Position);

	float2 nodeA_DirectionFloat{0.f};
	float2 nodeB_DirectionFloat{0.f};

	// create nodes if they don't exist yet

	if (nodeA_ID == TrackNodeID::Invalid)
	{
		nodeA_ID = CreateNode(nodeA_Position);
	}

	if (nodeB_ID == TrackNodeID::Invalid)
	{
		nodeB_ID = CreateNode(nodeB_Position);
	}

	// get/calculate direction

	if (nodeA_direction != TrackDirection::Empty)
	{
		nodeA_DirectionFloat = toFloat2(nodeA_direction);
	}
	else
	{
		// get it from connected segment
		// assumes it is correct since is valid should've checked this

		const TrackSegment& nodeA_ConnectingSegment = GetTrackSegment(nodeA_connection);

		if (nodeA_ConnectingSegment.nodeA == nodeA_ID)
		{
			nodeA_DirectionFloat = -nodeA_ConnectingSegment.nodeA_Direction;
		}
		else
		{
			nodeA_DirectionFloat = -nodeA_ConnectingSegment.nodeB_Direction;
		}
	}

	if (nodeB_direction != TrackDirection::Empty)
	{
		nodeB_DirectionFloat = -toFloat2(nodeB_direction);
	}
	else
	{
		// get it from connected segment
		// assumes it is correct since is valid should've checked this

		const TrackSegment& nodeB_ConnectingSegment = GetTrackSegment(nodeB_connection);

		if (nodeB_ConnectingSegment.nodeA == nodeB_ID)
		{
			nodeB_DirectionFloat = -nodeB_ConnectingSegment.nodeA_Direction;
		}
		else
		{
			nodeB_DirectionFloat = -nodeB_ConnectingSegment.nodeB_Direction;
		}
	}

	const TrackSegmentID newSegment = CreateSegment(nodeA_ID, nodeA_DirectionFloat, nodeB_ID, nodeB_DirectionFloat);

	if (nodeA_connection != TrackSegmentID::Invalid)
	{
		ConnectSegments(newSegment, nodeA_connection);
	}

	if (nodeB_connection != TrackSegmentID::Invalid)
	{
		ConnectSegments(newSegment, nodeB_connection);
	}

	return newSegment;
}

bool TrackManager::IsValidTrackPart( const float2& nodeA_Position, TrackDirection nodeA_direction, TrackSegmentID nodeA_connection, const float2& nodeB_Position, TrackDirection nodeB_direction, TrackSegmentID nodeB_connection )
{
	if (nodeA_direction == TrackDirection::Empty && nodeA_connection == TrackSegmentID::Invalid)
	{
		return false;
	}

	if (nodeB_direction == TrackDirection::Empty && nodeB_connection == TrackSegmentID::Invalid)
	{
		return false;
	}

	TrackNodeID nodeA_ID = GetNodeByPosition(nodeA_Position);
	TrackNodeID nodeB_ID = GetNodeByPosition(nodeB_Position);

	if (nodeA_ID == TrackNodeID::Invalid && (nodeA_connection != TrackSegmentID::Invalid || nodeA_direction == TrackDirection::Empty))
	{
		// it there is no node at that position how can you connected it to a previous segment
		return false;
	}

	if (nodeB_ID == TrackNodeID::Invalid && (nodeB_connection != TrackSegmentID::Invalid || nodeB_direction == TrackDirection::Empty))
	{
		// it there is no node at that position how can you connected it to a previous segment
		return false;
	}

	if (nodeA_ID != TrackNodeID::Invalid && nodeA_connection != TrackSegmentID::Invalid)
	{
		const TrackNode& nodeA = GetTrackNode(nodeA_ID);

		if (!nodeA.connectionLever.contains(nodeA_connection))
		{
			return false;
		}
	}

	if (nodeB_ID != TrackNodeID::Invalid && nodeB_connection != TrackSegmentID::Invalid)
	{
		const TrackNode& nodeB = GetTrackNode(nodeB_ID);

		if (!nodeB.connectionLever.contains(nodeB_connection))
		{
			return false;
		}
	}

	// TODO: long not finished
	// check for sharp corners and stuff liek that

	return true;
}

TrackNodeID TrackManager::GetNodeByPosition( const float2& position, float maxDifferance ) const
{
	const float maxDiffSQ = maxDifferance * maxDifferance;

	for (const auto& node : std::views::values(m_nodes))
	{
		if (sqrLength(node.nodePosition - position) < maxDiffSQ)
			return node.id;
	}

	return TrackNodeID::Invalid;
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
	const TrackNode node = GetTrackNode(segment.nodeB);

	const int connectionDir = node.connectionLever.at(id);
	if (connectionDir == -1) return TrackSegmentID::Invalid;

	return node.validConnections.at(id).at(connectionDir);
}

TrackSegmentID TrackManager::GetNextSegmentNegative( const TrackSegmentID id ) const
{
	const TrackSegment segment = GetTrackSegment(id);
	const TrackNode node = GetTrackNode(segment.nodeA);

	const int connectionDir = node.connectionLever.at(id);
	if (connectionDir == -1) return TrackSegmentID::Invalid;

	return node.validConnections.at(id).at(connectionDir);
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
