#include "precomp.h"
#include "Game/Trains/Wagon.h"
#include "Locomotive.h"

Locomotive::Locomotive( const Wagon& baseWagon ) : Wagon(baseWagon)
{
	m_wagonColor = 0xaa0000;
}
