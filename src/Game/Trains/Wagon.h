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

	[[nodiscard]] float GetVelocity() const { return m_velocity; }
	void SetVelocity( const float newSpeed ) { m_velocity = newSpeed; }

	[[nodiscard]] float GetAcceleration() const { return m_acceleration; }
	void SetAcceleration( const float newAccel ) { m_acceleration = newAccel; }

	void ImGuiDebugViewer() override;
	TrackWalker& GetFrontWalker() { return m_frontWalker; }
	TrackWalker& GetBackWalker() { return m_backWalker; }

	void SetInvincible( const bool invincible ) { m_invincible = invincible; }
	[[nodiscard]] float GetWagonLength() const { return m_wagonLength; }
private:
	TrackWalker m_frontWalker;
	TrackWalker m_backWalker;
	float m_wagonLength = 8.f; // Length in between the 2 bogeys
	float m_velocity{0.f};
	float m_acceleration{0.f};
	float m_maxTensionForce{1.f}; // Max force allowed to be exerted on the bogeys before derailing
	float m_dragCoefficient{1.5f}; // Amount of drag experienced by this wagon used for slowing down

	bool m_invincible{false}; // Stops derailing
	bool m_locked{false}; // kinematic movement of this wagon, i.e this train can only be moved by using Move()

	uint m_bogeyColor{0x494b50};
	uint m_wagonColor{0x8b96aa};
};
