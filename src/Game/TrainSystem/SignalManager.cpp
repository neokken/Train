#include "precomp.h"
#include "SignalManager.h"

#include <unordered_set>

#include "TrackManager.h"
#include "Renderables/CurvedSegment.h"

void SignalManager::Init( TrackManager& trackManager )
{
	m_trackManager = &trackManager;
}

SignalID SignalManager::BuildSignal( const TrackSegmentID segment, const float distanceOnSegment, const bool directionTowardsNodeB, const SignalType type )
{
	SignalID id = m_signalIDGenerator.GenerateID();
	m_signals.insert(std::pair(id, Signal(id, segment, distanceOnSegment, directionTowardsNodeB, type)));
	m_trackManager->AddSignal(segment, id);
	UpdateBlock(GetSignal(id));
	return id;
}

void SignalManager::RemoveSignal( const SignalID signal )
{
	m_trackManager->RemoveSignal(GetSignal(signal).segment, signal);
	m_signals.erase(signal);
}

void SignalManager::RemoveSegmentSignals( const TrackSegmentID trackId )
{
	std::vector<SignalID> signalsToRemove;
	for (auto& signal : m_signals)
	{
		if (signal.second.segment == trackId) signalsToRemove.push_back(signal.first);
	}
	for (auto& signal : signalsToRemove)
	{
		m_signals.erase(signal);
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

void SignalManager::UpdateBlock( const Signal& placedSignal )
{
	std::pair<std::vector<SignalID>, std::vector<SignalID>> forwardSignals = FindConnectedSignals(placedSignal.segment, placedSignal.directionTowardsNodeB, placedSignal.distanceOnSegment);
	std::pair<std::vector<SignalID>, std::vector<SignalID>> backwardsSignals = FindConnectedSignals(placedSignal.segment, !placedSignal.directionTowardsNodeB, placedSignal.distanceOnSegment);
	//find old blocks on this segment
	std::vector<SignalBlockID> oldblock;
	if (!forwardSignals.first.empty() && !backwardsSignals.first.empty())
	{
		oldblock = GetBlocksFromConnections(forwardSignals.first, backwardsSignals.first);
	}
	//Create new blocks
	if (!forwardSignals.first.empty())
	{
		const SignalBlockID blockID = CreateBlock();
		SignalBlock& block = GetMutableSignalBlock(blockID);
		std::vector<SignalID> forwardConnections;
		for (auto forwardSignal : forwardSignals.first)
		{
			if (GetSignal(forwardSignal).directionTowardsNodeB == placedSignal.directionTowardsNodeB)
				forwardConnections.push_back(forwardSignal);
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
					block.connections[from].push_back(to);
				}
			}
			RemoveBlock(mergeBlockID);
		}
	}
	if (!backwardsSignals.first.empty())
	{
		SignalBlockID blockID = CreateBlock();
		SignalBlock& block = GetMutableSignalBlock(blockID);
		std::vector<SignalID> backwardsConnections;
		for (auto backwardsSignal : backwardsSignals.first)
		{
			if (GetSignal(backwardsSignal).directionTowardsNodeB == placedSignal.directionTowardsNodeB)
				backwardsConnections.push_back(backwardsSignal);
		}
		block.connections.insert(std::pair(placedSignal.id, backwardsConnections));
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
					block.connections[from].push_back(to);
				}
			}

			RemoveBlock(mergeBlockID);
		}
	}

	//Clean up old blocks
	for (auto block : oldblock)
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
			directSignals.push_back(closest);
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
