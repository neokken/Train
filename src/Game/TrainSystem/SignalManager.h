#pragma once
#include "TrackSegment.h"

enum class SignalBlockID : uint32_t // NOLINT(performance-enum-size) since they are used as int types
{
	Invalid = 0,
};

enum class SignalID : uint32_t // NOLINT(performance-enum-size) since they are used as int types
{
	Invalid = 0,
};

struct SignalBlock
{
	unordered_map<SignalID, std::vector<SignalID>> connections;
};

enum class SignalType : uint32_t // NOLINT(performance-enum-size)
{
	Default,
	Chain
};

struct Signal
{
	TrackSegmentID segment = TrackSegmentID::Invalid;
	float distanceOnSegment;
	bool directionTowardsNodeB;
	SignalType type;
};
class SignalManager
{
public:
	SignalManager() = default;
private:
};
