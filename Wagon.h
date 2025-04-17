#pragma once
#include "Game/TrainSystem/TrainWalker/TrackWalker.h"
#include "GameObject/GameObject.h"

class Wagon :
	public Engine::GameObject
{
public:
	Wagon() = delete;
	explicit Wagon( const TrackWalker& trainWalker );

	void Update( float deltaTime ) override;

	void Render( const Engine::Camera& camera, Surface& target ) override;

	virtual void Derail();

	// TrackWalker specific code
	[[nodiscard]] float GetMoveSpeed() const { return m_moveSpeed; }
	void SetMoveSpeed( const float newSpeed ) { m_moveSpeed = newSpeed; }

	void ImGuiDebugViewer() override;

private:
	TrackWalker m_frontWalker;
	TrackWalker m_backWalker;
	float m_wagonLength = 8.f;
	float m_moveSpeed{0.f};
	float m_maxTensionForce{0.4f};

	uint m_bogeyColor{0x494b50};
	uint m_wagonColor{0x8b96aa};
};
