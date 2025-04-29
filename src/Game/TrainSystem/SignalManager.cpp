#include "precomp.h"
#include "SignalManager.h"

#include <unordered_set>

#include "TrackManager.h"
#include "Renderables/CurvedSegment.h"

void SignalManager::Init( TrackManager& trackManager )
{
	m_trackManager = &trackManager;
}

SignalID SignalManager::BuildSignal( const TrackSegmentID segment, const float distanceOnSegment, const bool directionTowardsNodeB, const SignalType type, const SignalID connectedSignal )
{
	SignalID id = m_signalIDGenerator.GenerateID();
	m_signals.insert(std::pair(id, Signal(id, segment, distanceOnSegment, directionTowardsNodeB, type, connectedSignal)));
	if (connectedSignal != SignalID::Invalid) GetMutableSignal(connectedSignal).oppositeSignal = id;
	m_trackManager->AddSignal(segment, id);
	UpdateBlock(GetMutableSignal(id));
	return id;
}

void SignalManager::RemoveSignal( const SignalID signalID )
{
	//std::printf("Removing signal: %d \n", static_cast<int>(signalID));
	const Signal signal = GetSignal(signalID);
	m_trackManager->RemoveSignal(signal.segment, signalID);
	m_signals.erase(signalID);

	if (IsValidBlock(signal.blockInFront))
	{
		auto block = GetBlock(signal.blockInFront);
		RemoveBlock(signal.blockInFront);
		for (const auto& connection : block.connections)
		{
			if (connection.first == signalID) continue;
			if (IsValidSignal(connection.first))
			{
				auto& signalBehind = GetMutableSignal(connection.first);
				if (signalBehind.directionTowardsNodeB == signal.directionTowardsNodeB)
				{
					if (IsValidBlock(signalBehind.blockBehind)) RemoveBlock(signalBehind.blockBehind);
				}
				else if (IsValidBlock(signalBehind.blockInFront)) RemoveBlock(signalBehind.blockInFront);

				UpdateBlock(signalBehind);
			}
		}
		for (const auto& connection : block.connections)
		{

			for (auto connected : connection.second)
			{
				if (IsValidSignal(connected))
				{
					auto& signalInFront = GetMutableSignal(connected);
					if (signalInFront.directionTowardsNodeB == signal.directionTowardsNodeB)
					{
						if (IsValidBlock(signalInFront.blockInFront)) RemoveBlock(signalInFront.blockInFront);
					}
					else if (IsValidBlock(signalInFront.blockBehind)) RemoveBlock(signalInFront.blockBehind);

					UpdateBlock(signalInFront);
				}
			}
		}
	}
	if (IsValidBlock(signal.blockBehind))
	{
		auto block = GetBlock(signal.blockBehind);
		RemoveBlock(signal.blockBehind);
		for (const auto& connection : block.connections)
		{
			if (IsValidSignal(connection.first))
			{
				auto& signalBehind = GetMutableSignal(connection.first);
				if (signalBehind.directionTowardsNodeB == signal.directionTowardsNodeB)
				{
					if (IsValidBlock(signalBehind.blockBehind)) RemoveBlock(signalBehind.blockBehind);
				}
				else if (IsValidBlock(signalBehind.blockInFront)) RemoveBlock(signalBehind.blockInFront);

				UpdateBlock(signalBehind);
			}
		}
		for (const auto& connection : block.connections)
		{
			for (auto connected : connection.second)
			{
				if (connected == signalID) continue;
				if (IsValidSignal(connected))
				{
					auto& signalInFront = GetMutableSignal(connected);
					if (signalInFront.directionTowardsNodeB == signal.directionTowardsNodeB)
					{
						if (IsValidBlock(signalInFront.blockInFront)) RemoveBlock(signalInFront.blockInFront);
					}
					else if (IsValidBlock(signalInFront.blockBehind)) RemoveBlock(signalInFront.blockBehind);

					UpdateBlock(signalInFront);
				}
			}
		}
	}
}

void SignalManager::RemoveSegmentSignalsAndBlocks( const TrackSegmentID trackId )
{
	std::vector<SignalID> signalsToRemove;
	for (auto& signal : m_signals)
	{
		if (signal.second.segment == trackId) signalsToRemove.push_back(signal.first);
	}
	for (auto& signal : signalsToRemove)
	{
		RemoveSignal(signal);
	}
}

const Signal& SignalManager::GetSignal( const SignalID signalID ) const
{
	DEBUG_ASSERT(IsValidSignal(signalID), "Signal ID is invalid");
	return m_signals.at(signalID);
}

const SignalBlock& SignalManager::GetBlock( const SignalBlockID blockID ) const
{
	DEBUG_ASSERT(IsValidBlock(blockID), "Block ID is invalid");
	return m_blocks.at(blockID);
}

SignalID SignalManager::FindClosestSignal( const float2 position, const float maxSqrDistance, const bool returnFirst ) const
{
	SignalID closest = SignalID::Invalid;
	float closestDist = INFINITY;
	for (auto& signal : std::views::values(m_signals))
	{
		const TrackSegment& seg = m_trackManager->GetTrackSegment(signal.segment);
		Engine::CurveData curve = {seg.nodeA_Position, seg.nodeA_Direction, seg.nodeB_Position, seg.nodeB_Direction};

		float2 pos = Engine::CurvedSegment::GetPositionOnCurvedSegment(signal.distanceOnSegment, curve);
		float2 dir = Engine::CurvedSegment::GetDirectionOnCurvedSegment(signal.distanceOnSegment, curve);
		pos += signal.directionTowardsNodeB ? float2(-dir.y, dir.x) : float2(dir.y, -dir.x) * 1.75f;
		float dist = sqrLength(position - pos);
		if (dist < maxSqrDistance)
		{
			if (returnFirst) return signal.id;
			if (dist < closestDist)
			{
				closest = signal.id;
				closestDist = dist;
			}
		}
	}
	return closest;
}

bool SignalManager::IsValidSignal( const SignalID id ) const
{
	return id != SignalID::Invalid && m_signals.contains(id);
}

bool SignalManager::IsValidBlock( const SignalBlockID id ) const
{
	return id != SignalBlockID::Invalid && m_blocks.contains(id);
}

void SignalManager::EnterBlock( SignalBlockID blockID, TrainID trainID )
{
	DEBUG_ASSERT(IsValidBlock(blockID), "Block ID is invalid!");
	GetMutableSignalBlock(blockID).containingTrains.insert(trainID);
}

void SignalManager::ExitBlock( SignalBlockID blockID, TrainID trainID )
{
	DEBUG_ASSERT(IsValidBlock(blockID), "Block ID is invalid!");
	auto& block = GetMutableSignalBlock(blockID);
	if (block.containingTrains.contains(trainID)) block.containingTrains.erase(trainID);
	else
	{
		Engine::Logger::Warn("Tried to remove train {} from block {} But train was not part of the block\n", static_cast<int>(trainID), static_cast<int>(blockID));
	}
}

SignalPassState SignalManager::GetSignalPassState( const SignalID signalID ) const
{
	DEBUG_ASSERT(IsValidSignal(signalID), "Invalid Signal ID");
	const auto& signal = GetSignal(signalID);

	if (!IsValidBlock(signal.blockInFront)) return SignalPassState::Closed;

	const auto& block = GetBlock(signal.blockInFront);

	//TODO: Chain signals

	if (!block.containingTrains.empty() || block.connections.at(signalID).empty())
	{
		return SignalPassState::Closed;
	}
	else
	{
		return SignalPassState::Open;
	}
}

SignalBlockID SignalManager::CreateBlock()
{
	SignalBlockID blockID = m_blockIDGenerator.GenerateID();
	m_blocks.insert(std::make_pair(blockID, SignalBlock()));
	m_blocks.at(blockID).id = blockID;
	return blockID;
}

void SignalManager::RemoveBlock( const SignalBlockID id )
{
	DEBUG_ASSERT(IsValidBlock(id), "Tried to remove block that doesnt exist");
	m_blocks.erase(id);
}

std::vector<SignalBlockID> SignalManager::GetBlocksFromConnections( const std::vector<SignalID>& outGoingSignal, const std::vector<SignalID>& incomingSignals ) const
{
	std::vector<SignalBlockID> found;
	//Find existing block
	for (const auto& block : m_blocks)
	{
		for (const auto& connections : block.second.connections)
		{
			for (const auto signal : connections.second)
			{
				for (const auto frontSignal : outGoingSignal)
				{
					for (const auto backSignal : incomingSignals)
					{
						if ((connections.first == frontSignal && signal == backSignal)
							|| signal == frontSignal && connections.first == backSignal)
						{
							if (std::find(found.begin(), found.end(), block.first) == found.end())
								found.push_back(block.first);
						}
					}
				}
			}
		}
	}
	return found;
}

SignalBlock& SignalManager::GetMutableSignalBlock( const SignalBlockID block )
{
	DEBUG_ASSERT(IsValidBlock(block), "Block ID Does not exist");
	return m_blocks.at(block);
}

Signal& SignalManager::GetMutableSignal( SignalID signal )
{
	DEBUG_ASSERT(IsValidSignal(signal), "Signal ID Does not exist");
	return m_signals.at(signal);
}

void SignalManager::UpdateBlock( Signal& placedSignal )
{
	//std::printf("Updating Signal: %d's blocks \n", static_cast<int>(placedSignal.id));

	std::pair<std::vector<SignalID>, std::vector<SignalID>> forwardSignals = FindConnectedSignals(placedSignal.segment, placedSignal.directionTowardsNodeB, placedSignal.distanceOnSegment);
	std::pair<std::vector<SignalID>, std::vector<SignalID>> backwardsSignals = FindConnectedSignals(placedSignal.segment, !placedSignal.directionTowardsNodeB, placedSignal.distanceOnSegment);
	//find old blocks on this segment
	std::vector<SignalBlockID> oldBlocks;
	if (!forwardSignals.first.empty() && !backwardsSignals.first.empty())
	{
		oldBlocks = GetBlocksFromConnections(forwardSignals.first, backwardsSignals.first);
	}
	std::vector<SignalBlockID> oneWayBlocks;
	if (placedSignal.oppositeSignal != SignalID::Invalid)
	{
		for (const auto& block : std::views::values(m_blocks))
		{
			for (const auto& connection : block.connections)
			{
				if (connection.first == placedSignal.oppositeSignal)
				{
					oldBlocks.push_back(block.id);
				}
				else
				{
					for (auto connected : connection.second)
					{
						if (connected == placedSignal.oppositeSignal)
						{
							oldBlocks.push_back(block.id);
						}
					}
				}
			}
		}
	}

	//Create new blocks
	if (!forwardSignals.first.empty())
	{
		const SignalBlockID blockID = CreateBlock();
		placedSignal.blockInFront = blockID;
		if (placedSignal.oppositeSignal != SignalID::Invalid) GetMutableSignal(placedSignal.oppositeSignal).blockBehind = blockID;

		SignalBlock& block = GetMutableSignalBlock(blockID);
		std::vector<SignalID> forwardConnections;
		for (auto forwardSignal : forwardSignals.first)
		{
			if (GetSignal(forwardSignal).directionTowardsNodeB == placedSignal.directionTowardsNodeB)
			{
				auto& sig = GetMutableSignal(forwardSignal);
				sig.blockBehind = blockID;
				if (sig.directionTowardsNodeB == placedSignal.directionTowardsNodeB)
				{
					sig.blockBehind = blockID;
					if (IsValidSignal(sig.oppositeSignal)) GetMutableSignal(sig.oppositeSignal).blockInFront = blockID;
				}
				else
				{
					sig.blockInFront = blockID;
					if (IsValidSignal(sig.oppositeSignal)) GetMutableSignal(sig.oppositeSignal).blockBehind = blockID;
				}
				if (IsValidSignal(sig.oppositeSignal)) GetMutableSignal(sig.oppositeSignal).blockInFront = blockID;

				forwardConnections.push_back(forwardSignal);
			}
			else if (placedSignal.oppositeSignal != SignalID::Invalid)
			{
				auto& sig = GetMutableSignal(forwardSignal);
				if (sig.directionTowardsNodeB == placedSignal.directionTowardsNodeB)
				{
					sig.blockBehind = blockID;
					if (IsValidSignal(sig.oppositeSignal)) GetMutableSignal(sig.oppositeSignal).blockInFront = blockID;
				}
				else
				{
					sig.blockInFront = blockID;
					if (IsValidSignal(sig.oppositeSignal)) GetMutableSignal(sig.oppositeSignal).blockBehind = blockID;
				}
				block.connections.insert(std::pair<SignalID, std::vector<SignalID>>(forwardSignal, {placedSignal.oppositeSignal}));
			}
		}
		block.connections.insert(std::pair(placedSignal.id, forwardConnections));

		//Merge blocks that are half connected i.e a cross intersection
		std::vector<SignalBlockID> indirectBlocks = GetBlocksFromConnections(forwardSignals.second, forwardSignals.second);
		std::vector<SignalBlockID> indirectBlocks2 = GetBlocksFromConnections(forwardSignals.first, forwardSignals.second);
		indirectBlocks.insert(indirectBlocks.end(), indirectBlocks2.begin(), indirectBlocks2.end());
		for (auto mergeBlockID : indirectBlocks)
		{
			if (!IsValidBlock(mergeBlockID)) continue;
			SignalBlock mergeBlock = GetBlock(mergeBlockID);
			//Merge and make sure we clamp connections beyond this signal
			for (const auto& connection : mergeBlock.connections)
			{
				SignalID from = connection.first;
				auto iter = std::find(backwardsSignals.first.begin(), backwardsSignals.first.end(), connection.first);
				if (iter != backwardsSignals.first.end()) from = placedSignal.id;
				for (const auto& connected : connection.second)
				{
					SignalID to = connected;
					auto iterator = std::find(backwardsSignals.first.begin(), backwardsSignals.first.end(), connected);
					if (iterator != backwardsSignals.first.end()) to = placedSignal.id;
					if (connected != placedSignal.oppositeSignal && connection.first != placedSignal.oppositeSignal)
					{
						if (from != to
							&& std::ranges::find(backwardsSignals.second, from) == backwardsSignals.second.end()
							&& std::ranges::find(backwardsSignals.second, to) == backwardsSignals.second.end()
							&& IsValidSignal(to)
							&& IsValidSignal(from)
						)
						{
							if (ranges::find(block.connections[from], to) == block.connections[from].end())
							{
								block.connections[from].push_back(to);
							}
							GetMutableSignal(from).blockInFront = blockID;
							GetMutableSignal(to).blockBehind = blockID;
						}
					}
				}
			}
			oldBlocks.push_back(mergeBlockID);
		}
	}
	if (!backwardsSignals.first.empty())
	{
		SignalBlockID blockID = CreateBlock();
		placedSignal.blockBehind = blockID;
		if (placedSignal.oppositeSignal != SignalID::Invalid) GetMutableSignal(placedSignal.oppositeSignal).blockInFront = blockID;

		SignalBlock& block = GetMutableSignalBlock(blockID);
		std::vector<SignalID> backwardsConnections;
		for (auto backwardsSignal : backwardsSignals.first)
		{
			if (GetSignal(backwardsSignal).directionTowardsNodeB == placedSignal.directionTowardsNodeB)
			{
				auto& sig = GetMutableSignal(backwardsSignal);
				if (sig.directionTowardsNodeB == placedSignal.directionTowardsNodeB)
				{
					sig.blockInFront = blockID;
					if (sig.oppositeSignal != SignalID::Invalid) GetMutableSignal(sig.oppositeSignal).blockBehind = blockID;
				}
				else
				{
					sig.blockBehind = blockID;
					if (sig.oppositeSignal != SignalID::Invalid) GetMutableSignal(sig.oppositeSignal).blockInFront = blockID;
				}
				if (sig.oppositeSignal != SignalID::Invalid) GetMutableSignal(sig.oppositeSignal).blockBehind = blockID;
				block.connections.insert(std::pair<SignalID, std::vector<SignalID>>(backwardsSignal, {placedSignal.id}));
			}
			else if (placedSignal.oppositeSignal != SignalID::Invalid)
			{
				auto& sig = GetMutableSignal(backwardsSignal);
				if (sig.directionTowardsNodeB == placedSignal.directionTowardsNodeB)
				{
					sig.blockInFront = blockID;
					if (sig.oppositeSignal != SignalID::Invalid) GetMutableSignal(sig.oppositeSignal).blockBehind = blockID;
				}
				else
				{
					sig.blockBehind = blockID;
					if (sig.oppositeSignal != SignalID::Invalid) GetMutableSignal(sig.oppositeSignal).blockInFront = blockID;
				}
				backwardsConnections.push_back(backwardsSignal);
			}
		}
		if (placedSignal.oppositeSignal != SignalID::Invalid)
			block.connections.insert(std::pair(placedSignal.oppositeSignal, backwardsConnections));
		//Merge blocks that are half connected i.e a cross intersection
		std::vector<SignalBlockID> indirectBlocks = GetBlocksFromConnections(backwardsSignals.second, backwardsSignals.second);
		std::vector<SignalBlockID> indirectBlocks2 = GetBlocksFromConnections(backwardsSignals.first, backwardsSignals.second);
		indirectBlocks.insert(indirectBlocks.end(), indirectBlocks2.begin(), indirectBlocks2.end());

		for (auto mergeBlockID : indirectBlocks)
		{
			if (!IsValidBlock(mergeBlockID)) continue;
			SignalBlock mergeBlock = GetBlock(mergeBlockID);
			//Merge and make sure we clamp connections beyond this signal
			for (const auto& connection : mergeBlock.connections)
			{
				SignalID from = connection.first;
				auto iter = std::find(forwardSignals.first.begin(), forwardSignals.first.end(), connection.first);
				if (iter != forwardSignals.first.end()) from = placedSignal.id;
				for (const auto& connected : connection.second)
				{
					SignalID to = connected;
					auto iterator = std::find(forwardSignals.first.begin(), forwardSignals.first.end(), connected);
					if (iterator != forwardSignals.first.end()) to = placedSignal.id;
					if (connected != placedSignal.oppositeSignal && connection.first != placedSignal.oppositeSignal)
					{
						if (from != to
							&& std::ranges::find(forwardSignals.second, from) == forwardSignals.second.end()
							&& std::ranges::find(forwardSignals.second, to) == forwardSignals.second.end()
							&& IsValidSignal(to)
							&& IsValidSignal(from)
						)
						{
							if (ranges::find(block.connections[from], to) == block.connections[from].end())
							{
								block.connections[from].push_back(to);
							}
							GetMutableSignal(from).blockInFront = blockID;
							GetMutableSignal(to).blockBehind = blockID;
						}
					}
				}
			}
			oldBlocks.push_back(mergeBlockID);
		}
	}

	//Clean up old blocks
	for (auto block : oldBlocks)
	{
		if (IsValidBlock(block))
			RemoveBlock(block);
	}
}



std::pair<std::vector<SignalID>, std::vector<SignalID>> SignalManager::FindConnectedSignals(
	const TrackSegmentID segment,
	const bool directionTowardsB,
	const float distance,
	std::unordered_set<TrackSegmentID> visitedSegments )
{
	if (visitedSegments.contains(segment))
		return {{}, {}};
	visitedSegments.insert(segment);

	std::vector<SignalID> directSignals;
	std::vector<SignalID> indirectSignals;

	const TrackSegment& tSegment = m_trackManager->GetTrackSegment(segment);
	if (!tSegment.signals.empty())
	{
		float closestDist = directionTowardsB ? 2.f : -2.f;
		SignalID closest = SignalID::Invalid;
		for (auto signalID : tSegment.signals)
		{
			const Signal& signal = GetSignal(signalID);
			if (directionTowardsB)
			{
				if (signal.distanceOnSegment > distance && signal.distanceOnSegment < closestDist)
				{
					closest = signalID;
					closestDist = signal.distanceOnSegment;
				}
			}
			else
			{
				if (signal.distanceOnSegment < distance && signal.distanceOnSegment > closestDist)
				{
					closest = signalID;
					closestDist = signal.distanceOnSegment;
				}
			}
		}
		if (closest != SignalID::Invalid)
		{
			directSignals.push_back(closest);
			const Signal& closestSig = GetSignal(closest);
			if (closestSig.oppositeSignal != SignalID::Invalid) directSignals.push_back(closestSig.oppositeSignal);
		}
	}

	if (directSignals.empty())
	{
		const TrackNodeID nextNodeID = directionTowardsB ? tSegment.nodeB : tSegment.nodeA;
		const TrackNode& nextNode = m_trackManager->GetTrackNode(nextNodeID);
		for (const auto& connections : nextNode.validConnections)
		{
			for (auto segmentID : connections.second)
			{
				if (connections.first == segment)
				{
					const TrackSegment& connectedSegment = m_trackManager->GetTrackSegment(segmentID);

					bool connectedAtA = (connectedSegment.nodeA == nextNodeID);
					bool connectedAtB = (connectedSegment.nodeB == nextNodeID);
					bool direction = directionTowardsB ? connectedAtA : !connectedAtB;
					auto found = FindConnectedSignals(segmentID, direction, direction ? -0.001f : 1.001f, visitedSegments);

					directSignals.insert(directSignals.end(), found.first.begin(), found.first.end());
					indirectSignals.insert(indirectSignals.end(), found.second.begin(), found.second.end());
				}
				else if (segmentID != segment)
				{
					const TrackSegment& connectedSegment = m_trackManager->GetTrackSegment(segmentID);
					bool connectedAtA = (connectedSegment.nodeA == nextNodeID);
					bool connectedAtB = (connectedSegment.nodeB == nextNodeID);
					bool direction = directionTowardsB ? connectedAtA : !connectedAtB;
					auto found = FindConnectedSignals(segmentID, direction, direction ? -0.001f : 1.001f, visitedSegments);

					indirectSignals.insert(indirectSignals.end(), found.first.begin(), found.first.end());
					indirectSignals.insert(indirectSignals.end(), found.second.begin(), found.second.end());
				}
			}
		}
	}
	return {directSignals, indirectSignals};
}
