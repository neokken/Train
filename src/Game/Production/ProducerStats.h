#pragma once

#include "ProduceType.h"

namespace Game
{
	struct ProducerStats
	{
		/// Type
		ProduceType type = ProduceType::Unknown;

		/// Amount of ticks to trigger one production
		uint productionRate = 20;

		/// Amount of produce per one finished production
		uint producePerProduction = 1;

		/// Maximum amount of produce before being throttled
		uint produceLimit = 500;

		/// How many stations does the producer have
		uint stationSize = 1;
	};
}
