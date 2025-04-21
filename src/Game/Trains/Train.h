#pragma once
#include "Game/TrainSystem/TrainWalker/TrackWalker.h"
#include "GameObject/GameObject.h"

class Wagon;

class Train :
	public Engine::GameObject
{
public:
	Train() = delete;
	/**
	 * Constructs a train and sets all wagons to be behind wagons[0]
	 * @param wagons list of wagon objects, these should be part of the world too, does not take ownership
	 */
	explicit Train( const std::vector<Wagon*>& wagons );

	void Update( float deltaTime ) override;

	void Render( const Engine::Camera& camera, Surface& target ) override;

	[[nodiscard]] float GetVelocity() const { return m_velocity; }
	void SetVelocity( const float newSpeed ) { m_velocity = newSpeed; }

	void ImGuiDebugViewer() override;

private:
	std::vector<Wagon*> m_wagons{};
	float m_wagonSpacing{4.5f}; // length of the link in between wagons
	float m_acceleration{0.f};
	float m_velocity{0.f};

	uint m_wireColor{0x303040}; // Color of the wire in between wagons
};
