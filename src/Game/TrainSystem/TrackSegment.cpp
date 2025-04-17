#include "precomp.h"
#include "TrackSegment.h"

void TrackNode::AddConnection( const TrackSegmentID from, const TrackSegmentID to )
{
	if (validConnections[from].empty()) connectionLever[from] = 0;

	validConnections[from].push_back(to);
}
