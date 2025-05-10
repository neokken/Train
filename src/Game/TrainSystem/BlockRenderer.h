#pragma once
class TrackManager;

namespace Engine
{
	class Camera;
}

class SignalManager;

class BlockRenderer
{
public:
	BlockRenderer() = default;
	void Init( SignalManager& signalManager, TrackManager& trackManager );
	void Render( Engine::Camera& camera );
private:
	SignalManager* m_signalManager;
	TrackManager* m_trackManager;
};
