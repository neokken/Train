#include "precomp.h"
#include "TrackWalker.h"

#include "Game/TrainSystem/TrackManager.h"

void TrackWalker::Init( const TrackManager* tm )
{
	m_trackManager = tm;
}

bool TrackWalker::Move( const float distance )
{
	float remainingDistance = distance * (m_flipMoveDir ? -1.f : 1.f);

	while (remainingDistance != 0.f)
	{
		const TrackSegment& currentSegment = m_trackManager->GetTrackSegment(m_currentSegmentID);
		const float goalPosition = m_distance + remainingDistance;

		if (goalPosition < 0.f)
		{
			// Going toward node A andbeyond
			const TrackSegmentID nextSegementID = m_trackManager->GetNextSegmentNegative(m_currentSegmentID);

			if (nextSegementID == TrackSegmentID::Invalid)
			{
				m_distance = 0.f;
				return false;
			}

			const TrackSegment& nextSegement = m_trackManager->GetTrackSegment(nextSegementID);

			if (currentSegment.m_nodeA == nextSegement.m_nodeB)
			{
				remainingDistance += m_distance;

				m_currentSegmentID = nextSegementID;
				m_distance = m_trackManager->GetTrackSegment(m_currentSegmentID).m_distance;
			}
			else
			{
				// track is made in the flipped direction
				remainingDistance += m_distance;
				m_currentSegmentID = nextSegementID;

				m_distance = 0.f;
				m_flipMoveDir = !m_flipMoveDir;
				remainingDistance *= -1.f;
			}

			continue; // go next
		}

		if (goalPosition > currentSegment.m_distance)
		{
			const TrackSegmentID nextSegementID = m_trackManager->GetNextSegmentPositive(m_currentSegmentID);

			if (nextSegementID == TrackSegmentID::Invalid)
			{
				m_distance = currentSegment.m_distance;
				return false;
			}

			const TrackSegment& nextSegement = m_trackManager->GetTrackSegment(nextSegementID);

			if (currentSegment.m_nodeB == nextSegement.m_nodeA)
			{
				remainingDistance -= currentSegment.m_distance - m_distance;

				m_currentSegmentID = nextSegementID;
				m_distance = 0.f;
			}
			else
			{
				// track is made in the flipped direction
				remainingDistance -= currentSegment.m_distance - m_distance;
				m_currentSegmentID = nextSegementID;

				m_distance = m_trackManager->GetTrackSegment(m_currentSegmentID).m_distance;
				m_flipMoveDir = !m_flipMoveDir;
				remainingDistance *= -1.f;
			}
			continue;
		}

		// we are within the size of the track 

		remainingDistance = 0;
		m_distance = goalPosition;
	}

	return true;
}

float2 TrackWalker::GetPosition() const
{
	if (m_currentSegmentID == TrackSegmentID::Invalid)
	{
		Engine::Logger::Warn("Trying to calculate a position on a TrackSegment without being on a segment. Returned 0");
		return 0.f;
	}

	const TrackSegment& currentSegment = m_trackManager->GetTrackSegment(m_currentSegmentID);

	const TrackNode& nodeA = m_trackManager->GetTrackNode(currentSegment.m_nodeA);
	const TrackNode& nodeB = m_trackManager->GetTrackNode(currentSegment.m_nodeB);

	return lerp(nodeA.m_nodePosition, nodeB.m_nodePosition, m_distance / currentSegment.m_distance);
}

bool TrackWalker::IsValid() const
{
	bool valid = m_trackManager->DoesSegmentExists(m_currentSegmentID);
	valid &= m_distance <= m_trackManager->GetTrackSegment(m_currentSegmentID).m_distance;
	valid &= m_distance >= 0.f;

	return valid;
}

void TrackWalker::SetCurrentTrackSegment( const TrackSegmentID newSegmentID, const float newDistance )
{
	m_currentSegmentID = newSegmentID;
	m_distance = newDistance;

	if (!IsValid())
	{
		throw std::runtime_error("SetCurrentTrackSegment created an invalid TrackWalker");
	}
}

void TrackWalker::ImGuiDebugViewer()
{
	ImGui::Text("TrackManager: %p", m_trackManager);
	ImGui::Text("CurrentTrackSegment: %d", static_cast<int>(m_currentSegmentID));
	ImGui::Text("Distance on track: %.2f", m_distance);

	ImGui::BeginDisabled(true);
	ImGui::Checkbox("Current rails flipped", &m_flipMoveDir);
	ImGui::EndDisabled();
}
