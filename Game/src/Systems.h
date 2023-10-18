#pragma once

#include "common/component_registry.h"

namespace PMG {
	class PMGSystem {
	public:
		virtual void Update() = 0;

		// registry used by all systems, seems hacky no?
		static inline ComponentRegistry registry;
	};
}