#include "precomp.h"
#include "SignalManager.h"

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
	return id;
}

void SignalManager::RemoveSignal( const SignalID signal )
{
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
		RemoveSignal(signal);
	}
}

const Signal& SignalManager::GetSignal( SignalID signalID )
{
	DEBUG_ASSERT(IsValidSignal(signalID), "Signal ID is invalid");
	return m_signals.at(signalID);
}

SignalID SignalManager::FindClosestSignal( float2 position, float maxSqrDistance, bool returnFirst ) const
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
