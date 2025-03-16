#include "precomp.h"
#include "TrackSerializer.h"

using json = nlohmann::json;

// ** type Serialization **

// ** Track Node id Serialization **
void to_json( nlohmann::json& j, const TrackNodeID& id )
{
	j = static_cast<int>(id);
}

void from_json( const nlohmann::json& j, TrackNodeID& id )
{
	id = static_cast<TrackNodeID>(j.get<int>());
}

// ** Segment Node id Serialization **
void to_json( nlohmann::json& j, const TrackSegmentID& id )
{
	j = static_cast<int>(id);
}

void from_json( const nlohmann::json& j, TrackSegmentID& id )
{
	id = static_cast<TrackSegmentID>(j.get<int>());
}

// ** DATA Serialization **

// ** Track Node Serialization **
void to_json( nlohmann::json& j, const TrackNode& node )
{
	j = json{
		{"connectionLever", node.m_connectionLever},
		{"validConnections", node.m_validConnections},
		{"nodePosition", {node.m_nodePosition.x, node.m_nodePosition.y}},
		{"id", node.m_id}
	};
}

void from_json( const nlohmann::json& j, TrackNode& node )
{
	node.m_connectionLever = j.at("connectionLever").get<std::unordered_map<TrackSegmentID, int>>();
	node.m_validConnections = j.at("validConnections").get<std::unordered_map<TrackSegmentID, std::vector<TrackSegmentID>>>();

	std::vector<float> pos = j.at("nodePosition").get<std::vector<float>>();
	node.m_nodePosition = {pos[0], pos[1]};

	node.m_id = static_cast<TrackNodeID>(j.at("id").get<int>());
}

// ** Track Segment Serialization **

void to_json( nlohmann::json& j, const TrackSegment& segment )
{
	j = json{
		{"id", segment.m_id},
		{"nodeA", segment.m_nodeA},
		{"nodeB", segment.m_nodeB}
	};
}

void from_json( const nlohmann::json& j, TrackSegment& segment )
{
	segment.m_id = j.at("id").get<TrackSegmentID>();
	segment.m_nodeA = j.at("nodeA").get<TrackNodeID>();
	segment.m_nodeB = j.at("nodeB").get<TrackNodeID>();
}
