#pragma once
#include "Game/TrainSystem/TrainManager.h"
class Train;

namespace Engine
{
	class Camera;
}

class TrackManager;

class TrainDebugger
{
public:
	TrainDebugger() = default;
	void Init( TrackManager& trackManager, TrainManager& trainManager );
	void Update( Engine::Camera& camera, Surface& drawSurface );
	void UI();
private:
	TrainID m_selectedTrain{TrainID::Invalid};
	TrackManager* m_trackManager{nullptr};
	TrainManager* m_trainManager{nullptr};
};
