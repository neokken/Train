#include "precomp.h"
#include "TrackSegment.h"

void TrackNode::AddConnection( const TrackSegmentID from, const TrackSegmentID to )
{
	if (validConnections[from].empty()) connectionLever[from] = 0;

	validConnections[from].push_back(to);
}

void TrackNode::RemoveSegment( const TrackSegmentID id )
{
	connectionLever.erase(id);
	validConnections.erase(id);

	for (auto& [from, connections] : validConnections)
	{
		auto& vec = connections;
		vec.erase(ranges::remove(vec, id).begin(), vec.end());

		if (connectionLever.contains(from))
		{
			int& idx = connectionLever[from];
			if (idx >= static_cast<int>(vec.size()))
				idx = -1;
		}
	}
}
