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
	explicit Train( const std::vector<Wagon*>& wagons, TrackManager& trackManager );

	void Update( float deltaTime ) override;

	void Render( const Engine::Camera& camera, Surface& target ) override;

	[[nodiscard]] float GetVelocity() const { return m_velocity; }
	void SetVelocity( const float newSpeed ) { m_velocity = newSpeed; }

	void ImGuiDebugViewer() override;

	void VisualizeDebugInfo( const Engine::Camera& camera, Surface& screen, Engine::World& world ) const;

	void SetNavTarget( TrackSegmentID segment, float distanceOnSegment );



private:
	/**
	 * Returns the max distance the train will take to stop, the reality can be shorter due to environmental factors like track drag
	 * @return Distance in track units
	 */
	float GetMaxStoppingDistance() const;

	//Recalculate mass and accelerations of the whole train
	void CalculateWagons();
	TrackManager& m_trackManager;
	std::vector<Wagon*> m_wagons{};
	TrackSegmentID m_targetSegment;
	std::vector<int> m_currentPath;
	float m_targetDistanceOnTargetSegment;
	float m_wagonSpacing{4.5f}; // length of the link in between wagons
	float m_targetVelocity{0.f};
	float m_acceleration{0.f};
	float m_velocity{0.f};
	float m_targetDistance{0.f};

	float m_maxAccelerationForward{0.f};
	float m_maxAccelerationBackwards{0.f};
	float m_mass{0.f};
	float m_maxBrakingForce{0.f};
	bool m_braking{false};
	uint m_wireColor{0x303040}; // Color of the wire in between wagons

	//DEBUG
	int targetSegment;
};
