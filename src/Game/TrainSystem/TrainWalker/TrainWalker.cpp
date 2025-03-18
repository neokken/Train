#include "precomp.h"
#include "TrainWalker.h"

#include "Game/TrainSystem/TrackManager.h"

void TrainWalker::Init( const TrackManager* tm )
{
	m_trackManager = tm;
}

bool TrainWalker::Move( const float distance )
{
	float remainingDistance = distance;

	while (remainingDistance != 0.f)
	{
		const TrackSegment& currentSegment = m_trackManager->GetTrackSegment(m_currentSegmentID);
		const float goalPosition = m_distance - remainingDistance;

		if (goalPosition < 0.f)
		{
			const TrackSegmentID nextSegement = m_trackManager->GetNextSegmentNegative(m_currentSegmentID);

			if (nextSegement == TrackSegmentID::Invalid)
			{
				m_distance = 0.f;
				return false;
			}

			m_currentSegmentID = nextSegement;
			m_distance = m_trackManager->GetTrackSegment(m_currentSegmentID).m_distance;
			remainingDistance = goalPosition;
			continue; // go next
		}

		if (goalPosition > currentSegment.m_distance)
		{
			const TrackSegmentID nextSegement = m_trackManager->GetNextSegmentPositive(m_currentSegmentID);

			if (nextSegement == TrackSegmentID::Invalid)
			{
				m_distance = currentSegment.m_distance;
				return false;
			}

			m_currentSegmentID = nextSegement;
			m_distance = 0.f;
			remainingDistance = goalPosition;
			continue;
		}

		// we are within the size of the track 

		m_distance = goalPosition;
		remainingDistance = goalPosition;
	}

	return true;
}

float2 TrainWalker::GetPosition() const
{
	const TrackSegment& currentSegment = m_trackManager->GetTrackSegment(m_currentSegmentID);

	const TrackNode& nodeA = m_trackManager->GetTrackNode(currentSegment.m_nodeA);
	const TrackNode& nodeB = m_trackManager->GetTrackNode(currentSegment.m_nodeB);

	return lerp(nodeA.m_nodePosition, nodeB.m_nodePosition, m_distance / currentSegment.m_distance);
}

bool TrainWalker::IsValid() const
{
	return m_trackManager->DoesSegmentExists(m_currentSegmentID);
}
