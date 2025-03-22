#include "precomp.h"
#include "TrackSegment.h"

void TrackNode::AddConnection( const TrackSegmentID from, const TrackSegmentID to )
{
	if (m_validConnections[from].empty()) m_connectionLever[from] = 0;

	m_validConnections[from].push_back(to);
}
