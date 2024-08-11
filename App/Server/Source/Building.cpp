#include "Building.h"
#include "Game.h"
#include "Missile.h"

namespace PMG {
	const AttackableStats tower_stats = {
        0, // move_speed
		100, // health
		100, // max_health
		0, // experience
		0, // level
		0, // health regen
	};

	Building::Building(Team team) : Attackable(tower_stats) {
		prefab = UnitPrefab::TOWER;
		this->team = team;
        target_type = TargetType::BUILDING;
        basic_attack_info = {
            BasicAttackType::RANGED, // type
            500, // range
            5, // damage
            1, // attack speed
            0.25, // hit point
            0, // last attack
			700 // acquisition range
        };
	}
}