#pragma once
#include "Game/TrainSystem/TrainWalker/TrackWalker.h"
#include "GameObject/GameObject.h"

struct WagonMovementInfo
{
	float velocityChangeAmount; // Amount the velocity of the bogeys rotated i.e how hard of a corner the wagon is going through
};

class Wagon :
	public Engine::GameObject
{
public:
	Wagon() = delete;
	explicit Wagon( const TrackWalker& trainWalker );

	void Update( float deltaTime ) override;
	/**
	 * Move the wagon by a set amount
	 * @param distance 
	 * @param deltaTime deltaTime used for derailment calculations set to 0 for single uses
	 * @return 
	 */
	WagonMovementInfo Move( float distance, float deltaTime );
	void Render( const Engine::Camera& camera, Surface& target ) override;

	virtual void Derail();

	[[nodiscard]] float GetVelocity() const { return m_velocity; }
	void SetVelocity( const float newSpeed ) { m_velocity = newSpeed; }

	[[nodiscard]] float GetAcceleration() const { return m_acceleration; }
	void SetAcceleration( const float newAccel ) { m_acceleration = newAccel; }

	[[nodiscard]] float GetAirDragCoefficient() const { return m_airDragCoefficient; }

	void ImGuiDebugViewer() override;
	TrackWalker& GetFrontWalker() { return m_frontWalker; }
	TrackWalker& GetBackWalker() { return m_backWalker; }

	void SetInvincible( const bool invincible ) { m_invincible = invincible; }
	void SetLocked( const bool locked ) { m_locked = locked; }
	[[nodiscard]] float GetWagonLength() const { return m_wagonLength; }

private:
	TrackWalker m_frontWalker;
	TrackWalker m_backWalker;

	float m_wagonLength = 8.f; // Length in between the 2 bogeys
	float m_velocity{0.f};
	float m_acceleration{0.f};
	float m_airDragCoefficient{1.5f}; // Amount of drag experienced by this wagon from air (this only applies to the front wagon)
	float m_trackDragCoefficient{1.25f}; // Amount of drag experienced by this wagon from the track i.e hard bends etc.
	float m_maxTensionForce{1000.f}; // Max force allowed to be exerted on the bogeys before derailing
	float m_minTensionForce{0.3f}; // Min tensionforce required to affect train speed

	bool m_invincible{true}; // Stops derailing
	bool m_locked{false}; // kinematic movement of this wagon, i.e this train can only be moved by using Move()

	uint m_bogeyColor{0x494b50};
	uint m_wagonColor{0x8b96aa};
};
