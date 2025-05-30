﻿#include "precomp.h"
#include "TrackManager.h"

#include <queue>
#include <unordered_set>

#include "Renderables/CurvedSegment.h"
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

	newSegment.nodeA_Position = GetTrackNode(newSegment.nodeA).nodePosition;
	newSegment.nodeB_Position = GetTrackNode(newSegment.nodeB).nodePosition;;
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

	newSegment.distance = Engine::CurvedSegment::GetSegmentLength(Engine::CurveData{nodeA.nodePosition, nodeA_direction, nodeB.nodePosition, nodeB_direction});

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

void TrackManager::DeleteTrackPart( TrackSegmentID id )
{
	if (!DoesSegmentExists(id))
	{
		Engine::Logger::Warn("Trying to delete segment : {}, however segment id did not exist", static_cast<int>(id));
		return;
	}

	TrackNode& nodeA = GetMutableTrackNode(m_segments[id].nodeA);
	TrackNode& nodeB = GetMutableTrackNode(m_segments[id].nodeB);
	m_segments.erase(id);

	nodeA.RemoveSegment(id);
	nodeB.RemoveSegment(id);

	if (nodeA.validConnections.empty())
	{
		m_nodes.erase(nodeA.id);
	}

	if (nodeB.validConnections.empty())
	{
		m_nodes.erase(nodeB.id);
	}
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

TrackNodeID TrackManager::GetNodeByPosition( const float2& position, const float maxDifference, float* outDistance ) const
{
	float minDiff = maxDifference * maxDifference;
	TrackNodeID bestNode = TrackNodeID::Invalid;

	for (const auto& node : std::views::values(m_nodes))
	{
		const float distance = sqrLength(node.nodePosition - position);
		if (distance < minDiff)
		{
			minDiff = distance;
			bestNode = node.id;
		}
	}
	if (outDistance)
		*outDistance = std::sqrt(minDiff);

	return bestNode;
}

TrackSegmentID TrackManager::GetSegmentByPosition( const float2& position, const float maxDifference, float* outDistance ) const
{
	float minDiff = maxDifference * maxDifference;
	TrackSegmentID bestSegment = TrackSegmentID::Invalid;

	for (const auto& segment : std::views::values(m_segments))
	{
		Engine::CurveData data{.lStart = segment.nodeA_Position, .lStartDir = segment.nodeA_Direction, .lEnd = segment.nodeB_Position, .lEndDir = segment.nodeB_Direction};

		const float distance = sqrLength(Engine::CurvedSegment::GetClosestPoint(data, position) - position);
		if (distance < minDiff)
		{
			minDiff = distance;
			bestSegment = segment.id;
		}
	}

	if (outDistance)
		*outDistance = std::sqrt(minDiff);

	return bestSegment;
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

const TrackSegmentID TrackManager::GetTrackSegment( TrackNodeID a, TrackNodeID b ) const
{
	const TrackNode& nodeA = GetTrackNode(a);
	for (auto connectionList : nodeA.validConnections)
	{
		for (auto connection : connectionList.second)
		{
			const TrackSegment& segment = GetTrackSegment(connection);
			if (segment.nodeA == b
				|| segment.nodeB == b)
			{
				return connection;
			}
		}
	}
	return TrackSegmentID::Invalid;
}

const TrackSegmentID TrackManager::GetClosestTrackSegment( const float2 position, const float sqrMaxDistance, const bool returnFirstFound ) const
{
	TrackSegmentID closest = TrackSegmentID::Invalid;
	float closestDistance = INFINITY;
	for (const auto& segment : std::views::values(m_segments))
	{
		float distance = SQRDistancePointToSegment(position, segment);
		if (distance < sqrMaxDistance)
		{
			if (returnFirstFound) return segment.id;
			if (distance < closestDistance)
			{
				closest = segment.id;
				closestDistance = distance;
			}
		}
	}
	return closest;
}

float TrackManager::GetClosestDistanceOnTrackSegment( TrackSegmentID segmentID, float2 position ) const
{
	TrackSegment segment = GetTrackSegment(segmentID);
	const float2 nodeA_pos = GetTrackNode(segment.nodeA).nodePosition;
	const float2 nodeB_pos = GetTrackNode(segment.nodeB).nodePosition;

	const Engine::CurveData curveData{nodeA_pos, segment.nodeA_Direction, nodeB_pos, segment.nodeB_Direction};

	return Engine::CurvedSegment::GetClosestDistance(curveData, position);
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

float TrackManager::SQRDistancePointToSegment( const float2& position, const TrackSegment& segment ) const
{
	const float2 nodeA_pos = GetTrackNode(segment.nodeA).nodePosition;
	const float2 nodeB_pos = GetTrackNode(segment.nodeB).nodePosition;

	Engine::CurveData curveData{nodeA_pos, segment.nodeA_Direction, nodeB_pos, segment.nodeB_Direction};

	return sqrLength(Engine::CurvedSegment::GetClosestPoint(curveData, position) - position);
}

#pragma region AStar

std::vector<int> TrackManager::CalculatePath( const TrackSegmentID startID, bool startDirectionTowardsB, const TrackSegmentID targetID ) const
{
	if (!DoesSegmentExists(startID) || !DoesSegmentExists(targetID)) return {};

	std::vector<std::unique_ptr<AStarNode>> allNodes;

	struct CompareNodes
	{
		bool operator()( const AStarNode* a, const AStarNode* b ) const
		{
			return a->f() > b->f();
		}
	};

	std::priority_queue<AStarNode*, std::vector<AStarNode*>, CompareNodes> open; // Priority queue is automatically sorted
	unordered_map<TrackSegmentID, AStarNode*> openMap;
	std::vector<AStarNode*> closed;

	allNodes.push_back(std::make_unique<AStarNode>(startID, 0.f, PathHeuristic(GetTrackSegment(startID).nodeA, GetTrackSegment(targetID).nodeA), nullptr));
	AStarNode* startNode = allNodes.back().get();
	open.push(startNode);
	openMap.insert(std::pair(startID, startNode));

	while (!open.empty())
	{
		AStarNode* current = open.top();

		if (current->segment == targetID) return ReconstructPath(*current);

		open.pop();
		openMap.erase(current->segment);
		closed.push_back(current);

		const TrackSegment& currentSeg = GetTrackSegment(current->segment);
		TrackNodeID currentTNode;

		//Find next node in the correct direction
		if (current->parent != nullptr)
		{
			const TrackSegment& parentSeg = GetTrackSegment(current->parent->segment);
			if (parentSeg.nodeA == currentSeg.nodeA || parentSeg.nodeB == currentSeg.nodeA) currentTNode = currentSeg.nodeB;
			else currentTNode = currentSeg.nodeA;
		}
		else if (startDirectionTowardsB) currentTNode = currentSeg.nodeB;
		else currentTNode = currentSeg.nodeA;

		const std::vector<TrackSegmentID> neighbors = GetTrackNode(currentTNode).validConnections.at(current->segment);
		for (int i = 0; i < static_cast<int>(neighbors.size()); ++i)
		{
			TrackSegment connectingSegment = GetTrackSegment(neighbors[i]);

			bool nodeIsClosed = false;
			for (auto aStarNode : closed)
			{
				if (aStarNode->segment == neighbors[i]) // Node already visited
				{
					nodeIsClosed = true;
				}
			}
			if (nodeIsClosed) continue;
			float tentativeG = current->g + PathDistance(connectingSegment);

			AStarNode* neighborAStar;
			if (openMap.contains(neighbors[i]))
			{
				neighborAStar = openMap.at(neighbors[i]);
				if (tentativeG >= neighborAStar->g)
				{
					continue;
				}
				else
				{
					neighborAStar->parent = current;
					neighborAStar->parentLever = i;
					neighborAStar->g = tentativeG;
					TrackNodeID neighborNode = (currentSeg.nodeA == connectingSegment.nodeA || currentSeg.nodeB == connectingSegment.nodeA) ? connectingSegment.nodeB : connectingSegment.nodeA;
					neighborAStar->h = PathHeuristic(neighborNode, GetTrackSegment(targetID).nodeA);
					openMap.erase(neighbors[i]);
					openMap.insert(std::pair(neighbors[i], neighborAStar));
				}
			}
			else
			{
				TrackNodeID neighborNode = (currentSeg.nodeA == connectingSegment.nodeA || currentSeg.nodeB == connectingSegment.nodeA) ? connectingSegment.nodeB : connectingSegment.nodeA;
				allNodes.push_back(std::make_unique<AStarNode>(neighbors[i], tentativeG, PathHeuristic(neighborNode, GetTrackSegment(targetID).nodeA), current, i));
				neighborAStar = allNodes.back().get();
				open.push(neighborAStar);
				openMap.insert(std::pair(neighbors[i], neighborAStar));
			}
		}
	}
	return {}; // No path exists
}

void TrackManager::SetNodeLever( const TrackNodeID node, const TrackSegmentID segment, const int leverValue )
{
	GetMutableTrackNode(node).connectionLever.at(segment) = leverValue;
}

float TrackManager::PathHeuristic( const TrackNodeID a, const TrackNodeID b ) const
{
	return length(GetTrackNode(a).nodePosition - GetTrackNode(b).nodePosition);
}

float TrackManager::PathDistance( const TrackSegment& segment )
{
	//If we have more data of other trains etc we can add more here to incentivise different paths
	return segment.distance;
}

std::vector<int> TrackManager::ReconstructPath( const AStarNode& finalNode )
{
	std::vector<int> rpath;
	const AStarNode* current = &finalNode;
	while (current != nullptr)
	{
		rpath.push_back(current->parentLever);
		current = current->parent;
	}
	std::vector<int> path;
	for (int i = static_cast<int>(rpath.size()) - 2; i >= 0; --i)
	{
		path.push_back(rpath[i]);
	}
	return path;
}

#pragma endregion
