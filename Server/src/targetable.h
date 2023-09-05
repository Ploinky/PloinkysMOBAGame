#pragma once

#include "game_object.h"
namespace PMG {
	class Targetable : public GameObject {
	public:
		virtual bool IsTargetable() final { return true; };

		Physics::Vector3 position;
		Physics::Vector3 rotation;

		stats_t stats = {
			100,
			100,
			5
		};
	};
}