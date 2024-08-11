#pragma once

#include "Person.h"
#include "Spell.h"
#include "AreaSpell.h"
#include "Game.h"
#include "MissileSpell.h"
#include "Buff.h"

namespace PMG {
	class FootballPerson : public Person {
	public:
		FootballPerson();
	};

	class ThrowFootball : public Spell {
	public:
		ThrowFootball();
		virtual void OnCast(Client* game, Attackable* spell_owner, SpellTargetInfo* target_info) override;
	};

	class ThrowFootballMissileData : public MissileSpellData {
	public:
		ThrowFootballMissileData() {
			radius = 60.0f;
			speed = 3000;
			distance = 1000;
		};
	};

	class ThrowFootballMissile : public MissileSpell {
	public:
		ThrowFootballMissile(SpellTargetInfo* target_info, Attackable* owner) : MissileSpell(ThrowFootballMissileData(), owner, target_info) {};
		virtual void TargetHit(Client* game, Attackable* owner, Attackable* target) override;
	};

	class KnockedOutCold : public Buff {
	public:
		KnockedOutCold();
		virtual void Apply(AttackableStats* stats, int* status_enable, int* status_disable);
	};

	class HealPerson : public Spell {
	public:
		HealPerson();
		virtual void OnCast(Client* game, Attackable* spell_owner, SpellTargetInfo* target_info);
	};

	class DoBlastArea : public Spell {
	public:
		DoBlastArea();
		virtual void OnCast(Client* game, Attackable* spell_owner, SpellTargetInfo* target_info);
	};

	class BlastAreaData : public AreaSpellData {
	public:
		BlastAreaData() {
			radius = 3;
			tickrate = 2;
			duration = 1000;
			can_hit_allies = false;
			can_hit_buildings = false;
			can_hit_self = false;
		};
	};

	class BlastArea : public AreaSpell {
	public:
		BlastArea(SpellTargetInfo* target_info) : AreaSpell(BlastAreaData(), target_info) {
			collision_radius = 3;
		};
		virtual void TargetHit(Client* game, Attackable* owner, Attackable* target);
	};

	class MakeRunFast : public Spell {
	public:
		MakeRunFast();
		virtual void OnCast(Client* game, Attackable* spell_owner, SpellTargetInfo* target_info);
	};

	class RunFast : public Buff {
	public:
		RunFast();
		virtual void Apply(AttackableStats* stats, int* status_enable, int* status_disable);
	};
}