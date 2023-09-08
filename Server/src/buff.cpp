#include "buff.h"
#include "server.h"

namespace PMG {
	void Buff::Update(double dt) {
		if (total_duration == -1) {
			// no time limit for buff
			return;
		}

		remaining_duration -= dt * 1000.0;

		if (remaining_duration <= 0) {
			should_remove = true;
		}
	}
}