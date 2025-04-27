#pragma once
#include "TrackSegment.h"
#include "TrainManager.h"

class TrackManager;

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
	SignalBlockID id;
	TrainID containingTrain;
	TrainID reservedBy;
};

enum class SignalType : uint
{
	Default,
	Chain
};

struct Signal
{
	SignalID id;
	TrackSegmentID segment = TrackSegmentID::Invalid;
	float distanceOnSegment;
	bool directionTowardsNodeB;
	SignalType type;
};

class SignalManager
{
public:
	SignalManager() = default;
	void Init( TrackManager& trackManager );
	SignalID BuildSignal( TrackSegmentID segment, float distanceOnSegment, bool directionTowardsNodeB, SignalType type );
	void RemoveSignal( SignalID signal );
	void RemoveSegmentSignals( TrackSegmentID trackId );
	const Signal& GetSignal( SignalID signalID );
	[[nodiscard]] const std::unordered_map<SignalID, Signal>& GetSignalMap() const { return m_signals; }
	[[nodiscard]] SignalID FindClosestSignal( float2 position, float maxSqrDistance, bool returnFirst ) const;
	[[nodiscard]] bool IsValidSignal( SignalID id ) const;
private:
	std::unordered_map<SignalBlockID, SignalBlock> m_blocks;
	std::unordered_map<SignalID, Signal> m_signals;
	Engine::IDGenerator<SignalID> m_signalIDGenerator;
	Engine::IDGenerator<SignalBlockID> m_blockIDGenerator;
	TrackManager* m_trackManager;
};
