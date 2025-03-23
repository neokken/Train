#pragma once

enum class TrackNodeID : uint32_t // NOLINT(performance-enum-size) since they are used as int types
{
	Invalid = 0,
};

enum class TrackSegmentID : uint32_t // NOLINT(performance-enum-size) since they are used as int types
{
	Invalid = 0,
};

enum class TrackDirection : uint8_t
{
	Empty,
	Horizontal,
	Vertical,
	Diagonal_Negative, //  down left & top right
	Diagonal_Positive, //	top left, down right
};

struct TrackNode
{
	// gives the index in the vector which is currently the active one
	std::unordered_map<TrackSegmentID, int> connectionLever;
	std::unordered_map<TrackSegmentID, std::vector<TrackSegmentID>> validConnections;

	float2 nodePosition{0.f};
	TrackNodeID id{TrackNodeID::Invalid};

	void AddConnection( TrackSegmentID from, TrackSegmentID to );
};

struct TrackSegment
{
	float2 nodeA_Direction{0.f}; // points towards B
	float2 nodeB_Direction{0.f}; // points toward A

	TrackSegmentID id{TrackSegmentID::Invalid};

	TrackNodeID nodeA{TrackNodeID::Invalid};
	TrackNodeID nodeB{TrackNodeID::Invalid};

	float distance{0.f};
	// later stuff will be added like, distance, curvature etc...
};
