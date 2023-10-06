#include "football_person.h"
#include "missile_spell.h"
#include "game.h"
#include "particle_spawner.h"

namespace PMG {
	void ThrowFootball::OnCast(Client* game, Attackable* spell_owner, SpellTargetInfo* target_info) {
		ThrowFootballMissile* missile = new ThrowFootballMissile(target_info, spell_owner);
		missile->owner = spell_owner;
		missile->position = spell_owner->position;
		game->AddGameObject(missile);
	}
	
	void ThrowFootballMissile::TargetHit(Client* game, Attackable* owner, Attackable* target) {
		target->TakeDamage(10, owner);
	}

	ThrowFootball::ThrowFootball() {
		cast_point = 10;
		cooldown = 1000;
	}

	HealPerson::HealPerson() {
		cast_point = 300;
		cooldown = 3000;
		cast_animation = "idle";
	}

	KnockedOutCold::KnockedOutCold() {
		remaining_duration = 1000;
		total_duration = 1000;
	}

	void KnockedOutCold::Apply(AttackableStats* stats, int* status_enable, int* status_disable) {
		*status_enable |= STATUS_STUNNED;
	}

	void HealPerson::OnCast(Client* game, Attackable* spell_owner, SpellTargetInfo* target_info) {
		if (target_info->target == nullptr) {
			// TODO this seems bad
			return;
		}
		target_info->target->Heal(50, spell_owner);

		ParticleSpawner* part = new ParticleSpawner();
		part->attached_to = target_info->target;
		part->particle_name = "models/heal_person.pts";
		game->AddGameObject(part);
	}

	DoBlastArea::DoBlastArea() {
	}

	void DoBlastArea::OnCast(Client* game, Attackable* spell_owner, SpellTargetInfo* target_info) {
		BlastArea* blast_area = new BlastArea(target_info);
		blast_area->position = target_info->target_point;
		blast_area->owner = spell_owner;
		game->AddGameObject(blast_area);


		ParticleSpawner* part = new ParticleSpawner();
		part->attached_to = blast_area;
		part->particle_name = "models/blast_area.pts";
		game->AddGameObject(part);
	}

	void BlastArea::TargetHit(Client* game, Attackable* owner, Attackable* target) {
		target->TakeDamage(10, owner);
		target->buffs.push_back(new KnockedOutCold());
	}

	MakeRunFast::MakeRunFast() {
	}

	void MakeRunFast::OnCast(Client* game, Attackable* spell_owner, SpellTargetInfo* target_info) {
		target_info->target->buffs.push_back(new RunFast());
	}

	RunFast::RunFast() {
		remaining_duration = 5000;
		total_duration = 5000;
	}

	void RunFast::Apply(AttackableStats* stats, int* status_enable, int* status_disable) {
		stats->move_speed *= 2;
	}

	FootballPerson::FootballPerson() : Person() {
		target_type = TargetType::CHARACTER;
		prefab = UnitPrefab::FOOTBALL_PERSON;

		// Q
		spells.push_back(new ThrowFootball());

		// W
		HealPerson* hp = new HealPerson();
		hp->cooldown = 3000;
		spells.push_back(hp);

		// E
		DoBlastArea* blast_area = new DoBlastArea();
		blast_area->cooldown = 4000;
		spells.push_back(blast_area);

		// R
		MakeRunFast* make_run_fast = new MakeRunFast();
		make_run_fast->cooldown = 6000;
		spells.push_back(make_run_fast);
	}
}