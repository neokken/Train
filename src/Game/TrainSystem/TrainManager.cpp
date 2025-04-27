#include "precomp.h"
#include "TrainManager.h"
#include "Game/Trains/Train.h"

TrainID TrainManager::AddTrain( Train& train )
{
	TrainID id = m_idGenerator.GenerateID();
	m_trains.insert(std::pair(id, &train));
	return id;
}

void TrainManager::RemoveTrain( const TrainID id )
{
	m_trains.erase(id);
}

bool TrainManager::IsValidTrain( const TrainID id ) const
{
	if (id == TrainID::Invalid) return false;
	return m_trains.contains(id);
}

const Train& TrainManager::GetTrain( const TrainID id ) const
{
	DEBUG_ASSERT(IsValidTrain(id), "Train ID was invalid");
	return *m_trains.at(id);
}

void TrainManager::SetTrainDestination( const TrainID id, const TrackSegmentID segment, const float distanceOnSegment )
{
	DEBUG_ASSERT(IsValidTrain(id), "Train ID was invalid");
	Train& train = GetMutableTrain(id);
	train.SetNavTarget(segment, distanceOnSegment);
}

Train& TrainManager::GetMutableTrain( const TrainID id )
{
	DEBUG_ASSERT(IsValidTrain(id), "Train ID was invalid");
	return *m_trains.at(id);
}
