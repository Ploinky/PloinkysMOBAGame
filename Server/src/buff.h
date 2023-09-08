#pragma once

#include "component_registry.h"
#include "game_object.h"

namespace PMG {
	class Buff {
	public:
		void Update(double dt);
		virtual void Apply(stats_t* stats, int* status_enable, int* status_disable) = 0;

		int total_duration;
		int remaining_duration;
		bool should_remove = false;
	};
}