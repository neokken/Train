#pragma once
#include "Game/TrainSystem/TrainWalker/TrackWalker.h"
#include "GameObject/GameObject.h"

class TrackWalkerVisualizer final : public Engine::GameObject
{
public:
	TrackWalkerVisualizer() = delete;
	explicit TrackWalkerVisualizer( const TrackWalker& trainWalker );

	void Update( float deltaTime ) override;

	void Render( const Engine::Camera& camera ) override;

	// TrackWalker specific code
	[[nodiscard]] float GetMoveSpeed() const { return m_moveSpeed; }
	void SetMoveSpeed( const float newSpeed ) { m_moveSpeed = newSpeed; }

	void ImGuiDebugViewer() override;

private:
	TrackWalker m_walker;

	float m_moveSpeed{0.f};

	uint m_color{0xdda15e};
};
