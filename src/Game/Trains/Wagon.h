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
	void Move( float distance );
	void Render( const Engine::Camera& camera, Surface& target ) override;

	virtual void Derail();

	// TrackWalker specific code
	[[nodiscard]] float GetMoveSpeed() const { return m_moveSpeed; }
	void SetMoveSpeed( const float newSpeed ) { m_moveSpeed = newSpeed; }

	void ImGuiDebugViewer() override;
	TrackWalker& GetFrontWalker() { return m_frontWalker; }
	TrackWalker& GetBackWalker() { return m_backWalker; }

	void SetInvincible( const bool invincible ) { m_invincible = invincible; }
	float GetWagonLength() const { return m_wagonLength; }
private:
	TrackWalker m_frontWalker;
	TrackWalker m_backWalker;
	float m_wagonLength = 8.f; // Length in between the 2 bogeys
	float m_moveSpeed{0.f};
	float m_maxTensionForce{1.f}; // Max force allowed to be exerted on the bogeys before derailing

	bool m_invincible{false}; // Stops derailing

	uint m_bogeyColor{0x494b50};
	uint m_wagonColor{0x8b96aa};
};
