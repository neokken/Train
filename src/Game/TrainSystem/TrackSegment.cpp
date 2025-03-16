#include "precomp.h"
#include "TrackSegment.h"

void TrackNode::AddConnection( const TrackSegmentID from, const TrackSegmentID to )
{
	m_validConnections[from].push_back(to);

	if (!m_connectionLever.contains(from) || m_connectionLever[from] == -1) m_connectionLever[from] = 0;
}

void TrackNode::AddTrackSegment( TrackSegmentID segmentID )
{
	m_validConnections[segmentID];

	if (!m_connectionLever.contains(segmentID)) m_connectionLever[segmentID] = -1;
}
