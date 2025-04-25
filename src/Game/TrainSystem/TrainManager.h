#pragma once
#include <unordered_map>

#include "Helpers/IDGenerator.h"
class Train;

enum class TrainID : uint32_t // NOLINT(performance-enum-size) since they are used as int types
{
	Invalid = 0,
};

class TrainManager
{
public:
	TrainID AddTrain( Train& train );
	void RemoveTrain( TrainID id );
	[[nodiscard]] bool IsValidTrain( TrainID id ) const;
	const Train& GetTrain( TrainID id ) const;
	[[nodiscard]] const std::unordered_map<TrainID, Train*> GetTrainMap() const { return m_trains; }
private:
	//Note the TrainManager does not own these objects since they are part of the world
	std::unordered_map<TrainID, Train*> m_trains;
	Engine::IDGenerator<TrainID> m_idGenerator;
};
