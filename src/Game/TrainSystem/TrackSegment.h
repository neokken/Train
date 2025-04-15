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
	Vertical,
	SteepUpRight,
	DiagonalUpRight,
	ShallowUpRight,
	Horizontal,
	ShallowDownRight,
	DiagonalDownRight,
	SteepDownRight
};

const float2 directionVectors[] = {
	{0.f, 0.f},
	{0.0f, 1.0f}, // Vertical 
	{std::cos(-67.5f * PI / 180.0f), std::sin(-67.5f * PI / 180.0f)}, // SteepUpRight
	{std::cos(-45.0f * PI / 180.0f), std::sin(-45.0f * PI / 180.0f)}, // DiagonalUpRight 
	{std::cos(-22.5f * PI / 180.0f), std::sin(-22.5f * PI / 180.0f)}, // ShallowUpRight 
	{1.0f, 0.0f}, // Horizontal (90)
	{std::cos(22.5f * PI / 180.0f), std::sin(22.5f * PI / 180.0f)}, // ShallowDownRight 
	{std::cos(45.f * PI / 180.0f), std::sin(45.f * PI / 180.0f)}, // DiagonalDownRight 
	{std::cos(67.5f * PI / 180.0f), std::sin(67.5f * PI / 180.0f)} // SteepDownRight 
};

inline float2 toFloat2( TrackDirection dir )
{
	return directionVectors[static_cast<int>(dir)];
}

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
