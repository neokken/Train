#pragma once
#include "Game/TrainSystem/TrackSegment.h"
#include "Game/TrainSystem/TrainManager.h"
class TrackBuilder;
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
	void Init( TrackManager& trackManager, TrainManager& trainManager, TrackBuilder& trackBuilder );
	void Update( const Engine::Camera& camera );
	void UI( float deltaTime );

private:
	TrainID m_selectedTrain{TrainID::Invalid};
	TrackManager* m_trackManager{nullptr};
	TrainManager* m_trainManager{nullptr};
	TrackBuilder* m_trackBuilder{nullptr};
	bool m_hasSelectedTrain{false};
	bool m_showBrakeDistance{true};
	bool m_showDestination{true};
	bool m_advancedInfo{true};
	TrackSegmentID m_targetSegment{TrackSegmentID::Invalid};
	float m_targetDistance{0.f};
	float m_lastFrameVelocity{0.f};
	float m_acceleration{0.f};
};
