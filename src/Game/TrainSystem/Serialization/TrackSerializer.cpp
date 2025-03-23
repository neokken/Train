#include "precomp.h"
#include "TrackSerializer.h"

using json = nlohmann::json;

// ** type Serialization **

// ** Track Node id Serialization **
void to_json( nlohmann::json& j, const TrackNodeID& id )
{
	j = static_cast<uint32_t>(id);
}

void from_json( const nlohmann::json& j, TrackNodeID& id )
{
	id = static_cast<TrackNodeID>(j.get<uint32_t>());
}

// ** Segment Node id Serialization **
void to_json( nlohmann::json& j, const TrackSegmentID& id )
{
	j = static_cast<uint32_t>(id);
}

void from_json( const nlohmann::json& j, TrackSegmentID& id )
{
	id = static_cast<TrackSegmentID>(j.get<uint32_t>());
}

// ** DATA Serialization **

// ** Track Node Serialization **
void to_json( nlohmann::json& j, const TrackNode& node )
{
	j = json{
		{"connectionLever", node.connectionLever},
		{"validConnections", node.validConnections},
		{"nodePosition", {node.nodePosition.x, node.nodePosition.y}},
		{"id", node.id}
	};
}

void from_json( const nlohmann::json& j, TrackNode& node )
{
	node.connectionLever = j.at("connectionLever").get<std::unordered_map<TrackSegmentID, int>>();
	node.validConnections = j.at("validConnections").get<std::unordered_map<TrackSegmentID, std::vector<TrackSegmentID>>>();

	std::vector<float> pos = j.at("nodePosition").get<std::vector<float>>();
	node.nodePosition = {pos[0], pos[1]};

	node.id = j.at("id").get<TrackNodeID>();
}

// ** Track Segment Serialization **

void to_json( nlohmann::json& j, const TrackSegment& segment )
{
	j = json{
		{"id", segment.id},
		{"nodeA", segment.nodeA},
		{"nodeB", segment.nodeB},
		{"distance", segment.distance}
	};
}

void from_json( const nlohmann::json& j, TrackSegment& segment )
{
	segment.id = j.at("id").get<TrackSegmentID>();
	segment.nodeA = j.at("nodeA").get<TrackNodeID>();
	segment.nodeB = j.at("nodeB").get<TrackNodeID>();
	segment.distance = j.at("distance").get<float>();
}
