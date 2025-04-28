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
	SignalID oppositeSignal = SignalID::Invalid;
};

class SignalManager
{
public:
	SignalManager() = default;
	void Init( TrackManager& trackManager );
	SignalID BuildSignal( TrackSegmentID segment, float distanceOnSegment, bool directionTowardsNodeB, SignalType type );
	void RemoveSignal( SignalID signal );
	void RemoveSegmentSignals( TrackSegmentID trackId );
	const Signal& GetSignal( SignalID signalID ) const;
	const SignalBlock& GetBlock( SignalBlockID blockID ) const;
	[[nodiscard]] const std::unordered_map<SignalID, Signal>& GetSignalMap() const { return m_signals; }
	[[nodiscard]] const std::unordered_map<SignalBlockID, SignalBlock>& GetBlockMap() const { return m_blocks; }
	[[nodiscard]] SignalID FindClosestSignal( float2 position, float maxSqrDistance, bool returnFirst ) const;
	[[nodiscard]] bool IsValidSignal( SignalID id ) const;
	[[nodiscard]] bool IsValidBlock( SignalBlockID id ) const;

private:
	SignalBlockID CreateBlock();
	void RemoveBlock( SignalBlockID id );
	std::vector<SignalBlockID> GetBlocksFromConnections( const std::vector<SignalID>& outGoingSignal, const std::vector<SignalID>& incomingSignals ) const;
	SignalBlock& GetMutableSignalBlock( SignalBlockID block );
	void UpdateBlock( const Signal& placedSignal );
	/**
	 * Find all signals that are in this direction on the track
	 * @param segment segment to start from
	 * @param directionTowardsB 
	 * @param distance distance to start from 
	 * @return first: directly connected signals \n
	 * second: indirectly connected signals
	 */
	std::pair<std::vector<SignalID>, std::vector<SignalID>> FindConnectedSignals( const TrackSegmentID segment,
	                                                                              const bool directionTowardsB,
	                                                                              const float distance,
	                                                                              std::unordered_set<TrackSegmentID> visitedSegments = {} );
	std::unordered_map<SignalBlockID, SignalBlock> m_blocks;
	std::unordered_map<SignalID, Signal> m_signals;
	Engine::IDGenerator<SignalID> m_signalIDGenerator;
	Engine::IDGenerator<SignalBlockID> m_blockIDGenerator;
	TrackManager* m_trackManager;
};
