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
	E,
	ESE,
	SE,
	SSE,
	S,
	SSW,
	SW,
	WSW,
	W,
	WNW,
	NW,
	NNW,
	N,
	NNE,
	NE,
	ENE,
	Count,
	Empty
};

constexpr float DegToRad( const float degrees )
{
	return degrees * (PI / 180.0f);
}

const float2 directionVectors[] = {
	float2(cos(DegToRad(0.f)), sin(DegToRad(0.f))), // E
	float2(cos(DegToRad(22.5f)), sin(DegToRad(22.5f))), // ESE
	float2(cos(DegToRad(45.f)), sin(DegToRad(45.f))), // SE
	float2(cos(DegToRad(67.5f)), sin(DegToRad(67.5f))), // SSE
	float2(cos(DegToRad(90.f)), sin(DegToRad(90.f))), // S
	float2(cos(DegToRad(112.5f)), sin(DegToRad(112.5f))), // SSW
	float2(cos(DegToRad(135.f)), sin(DegToRad(135.f))), // SW
	float2(cos(DegToRad(157.5f)), sin(DegToRad(157.5f))), // WSW
	float2(cos(DegToRad(180.f)), sin(DegToRad(180.f))), // W
	float2(cos(DegToRad(202.5f)), sin(DegToRad(202.5f))), // WNW
	float2(cos(DegToRad(225.f)), sin(DegToRad(225.f))), // NW
	float2(cos(DegToRad(247.5f)), sin(DegToRad(247.5f))), // NNW
	float2(cos(DegToRad(270.f)), sin(DegToRad(270.f))), // N
	float2(cos(DegToRad(292.5f)), sin(DegToRad(292.5f))), // NNE
	float2(cos(DegToRad(315.f)), sin(DegToRad(315.f))), // NE
	float2(cos(DegToRad(337.5f)), sin(DegToRad(337.5f))), // ENE
};

inline float2 toFloat2( TrackDirection dir )
{
	return directionVectors[static_cast<int>(dir)];
}

inline TrackDirection toTrackDir( const float2& dir )
{
	const float angle = fmodf(std::atan2(dir.y, dir.x) * (180.0f / PI) + 360.f, 360.f);

	return static_cast<TrackDirection>(static_cast<int>(round(angle / 22.5f)));
}

struct TrackNode
{
	// gives the index in the vector which is currently the active one
	std::unordered_map<TrackSegmentID, int> connectionLever;
	std::unordered_map<TrackSegmentID, std::vector<TrackSegmentID>> validConnections;

	float2 nodePosition{0.f};
	TrackNodeID id{TrackNodeID::Invalid};

	void AddConnection( TrackSegmentID from, TrackSegmentID to );

	void RemoveSegment( TrackSegmentID id );
};

struct TrackSegment
{
	float2 nodeA_Position{0.f};
	float2 nodeB_Position{0.f};

	float2 nodeA_Direction{0.f}; // points towards B
	float2 nodeB_Direction{0.f}; // points toward A

	TrackSegmentID id{TrackSegmentID::Invalid};

	TrackNodeID nodeA{TrackNodeID::Invalid};
	TrackNodeID nodeB{TrackNodeID::Invalid};

	float distance{0.f};
};
