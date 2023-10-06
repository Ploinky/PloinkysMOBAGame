#pragma once

#include "pmg_physics.h"
#include <vector>
#include "game_object.h"

namespace PMG {
	class Attackable;
	class Client;

	class SpellTargetInfo {
	public:
		Attackable* target = nullptr;
		Physics::Vector3 target_point;
	};

	class Spell {
	public:
		// point at which spell is cast in ms
		int cast_point = 100;
		int cooldown = 5000;
		int remaining_cooldown = -1;

		std::string cast_animation;
		
		void CastStart(Client* game, Attackable* spell_owner, SpellTargetInfo* target_info);

		void Cast(Client* game, Attackable* spell_owner, SpellTargetInfo* target_info);
		virtual void OnCast(Client* game, Attackable* spell_owner, SpellTargetInfo* target_info) = 0;
	};
}