#pragma once

enum class TrackNodeID : uint32_t // NOLINT(performance-enum-size) since they are used as int types
{
	Invalid = 0,
};

enum class TrackSegmentID : uint32_t // NOLINT(performance-enum-size) since they are used as int types
{
	Invalid = 0,
};

struct TrackNode
{
	// gives the index in the vector which is currently the active one
	std::unordered_map<TrackSegmentID, int> m_connectionLever;
	std::unordered_map<TrackSegmentID, std::vector<TrackSegmentID>> m_validConnections;

	float2 m_nodePosition{0.f};
	TrackNodeID m_id{TrackNodeID::Invalid};

	void AddConnection( TrackSegmentID from, TrackSegmentID to );
};

struct TrackSegment
{
	TrackSegmentID m_id{TrackSegmentID::Invalid};

	TrackNodeID m_nodeA{TrackNodeID::Invalid};
	TrackNodeID m_nodeB{TrackNodeID::Invalid};

	float m_distance{0.f};
	// later stuff will be added like, distance, curvature etc...
};
