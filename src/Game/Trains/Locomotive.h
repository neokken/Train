#pragma once
#include "Wagon.h"

class Locomotive :
	public Wagon
{
public:
	Locomotive() = delete;
	explicit Locomotive( const Wagon& baseWagon );

	float GetMaxForwardAcceleration() const { return m_maxForwardAcceleration; }
	float GetMaxBackwardsAcceleration() const { return m_maxBackwardsAcceleration; }

private:
	//Acceleration in kn
	float m_maxForwardAcceleration{250.f};
	float m_maxBackwardsAcceleration{50.f};
};
