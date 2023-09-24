#pragma once

#include "person.h"
#include "spell.h"
#include "area_spell.h"
#include "game.h"
#include "missile_spell.h"
#include "buff.h"

namespace PMG {
	class FootballPerson : public Person {
	public:
		FootballPerson();
	};

	class ThrowFootball : public Spell {
	public:
		ThrowFootball();
		virtual void OnCast(Game* game, Attackable* spell_owner, SpellTargetInfo* target_info) override;
	};

	class ThrowFootballMissileData : public MissileSpellData {
	public:
		ThrowFootballMissileData() {
			radius = 3;
			speed = 30;
			distance = 10;
		};
	};

	class ThrowFootballMissile : public MissileSpell {
	public:
		ThrowFootballMissile(SpellTargetInfo* target_info, Attackable* owner) : MissileSpell(ThrowFootballMissileData(), owner, target_info) {};
		virtual void TargetHit(Game* game, Attackable* owner, Attackable* target) override;
	};

	class KnockedOutCold : public Buff {
	public:
		KnockedOutCold();
		virtual void Apply(stats_t* stats, int* status_enable, int* status_disable);
	};

	class HealPerson : public Spell {
	public:
		HealPerson();
		virtual void OnCast(Game* game, Attackable* spell_owner, SpellTargetInfo* target_info);
	};

	class DoBlastArea : public Spell {
	public:
		DoBlastArea();
		virtual void OnCast(Game* game, Attackable* spell_owner, SpellTargetInfo* target_info);
	};

	class BlastAreaData : public AreaSpellData {
	public:
		BlastAreaData() {
			radius = 3;
			tickrate = 2;
			duration = 1000;
			can_hit_allies = true;
			can_hit_buildings = false;
		};
	};

	class BlastArea : public AreaSpell {
	public:
		BlastArea(SpellTargetInfo* target_info) : AreaSpell(BlastAreaData(), target_info) {
			collision_radius = 3;
		};
		virtual void TargetHit(Game* game, Attackable* owner, Attackable* target);
	};

	class MakeRunFast : public Spell {
	public:
		MakeRunFast();
		virtual void OnCast(Game* game, Attackable* spell_owner, SpellTargetInfo* target_info);
	};

	class RunFast : public Buff {
	public:
		RunFast();
		virtual void Apply(stats_t* stats, int* status_enable, int* status_disable);
	};
}