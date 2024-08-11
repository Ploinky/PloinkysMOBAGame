#pragma once

#include <Common/PMG_Common.h>

namespace PMG {
	class PMGSystem {
	public:
		virtual void Update() = 0;

		// registry used by all systems, seems hacky no?
		static inline ComponentRegistry registry;
	};
}