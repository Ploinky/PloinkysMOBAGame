#include "football_person.h"
#include "missile_spell.h"
#include "game.h"

namespace PMG {
	void ThrowFootball::OnCast(Game* game, GameObject* spell_owner, SpellTargetInfo* target_info) {
		ThrowFootballMissile* missile = new ThrowFootballMissile(target_info, spell_owner);
		missile->owner = spell_owner;
		missile->position = spell_owner->position;
		game->AddGameObject(missile);

		Networking::SpawnPacket* spawn = new Networking::SpawnPacket();
		spawn->unit = missile->unit_id;
		spawn->unit_type = UnitPrefab::THROW_FOOTBALL;
		spawn->team = missile->team;
		spawn->x = missile->position.x;
		spawn->y = missile->position.y;
		game->SendPacket(spawn);
	}
	
	void ThrowFootballMissile::TargetHit(Game* game, GameObject* owner, GameObject* target) {
		target->TakeDamage(game, 10, owner);
	}

	ThrowFootball::ThrowFootball() {
		cast_point = 10;
		cooldown = 1000;
	}

	HealPerson::HealPerson() {
		cast_point = 300;
		cooldown = 3000;
	}

	KnockedOutCold::KnockedOutCold() {
		remaining_duration = 1000;
		total_duration = 1000;
	}

	void KnockedOutCold::Apply(stats_t* stats, int* status_enable, int* status_disable) {
		*status_enable |= STATUS_STUNNED;
	}

	void HealPerson::OnCast(Game* game, GameObject* spell_owner, SpellTargetInfo* target_info) {
		if (target_info->target == nullptr) {
			// TODO this seems bad
			return;
		}
		game->Heal(target_info->target, 50);

		Networking::PlayParticlePacket* pck = new Networking::PlayParticlePacket();
		pck->unit = target_info->target->unit_id;
		pck->particle = "test";
		game->SendPacket(pck);
	}

	DoBlastArea::DoBlastArea() {
	}

	void DoBlastArea::OnCast(Game* game, GameObject* spell_owner, SpellTargetInfo* target_info) {
		BlastArea* blast_area = new BlastArea(target_info);
		blast_area->position = target_info->target_point;
		blast_area->owner = spell_owner;

		game->AddGameObject(blast_area);
	}

	void BlastArea::TargetHit(Game* game, GameObject* owner, GameObject* target) {
		target->TakeDamage(game, 10, owner);
		target->buffs.push_back(new KnockedOutCold());
	}

	MakeRunFast::MakeRunFast() {
	}

	void MakeRunFast::OnCast(Game* game, GameObject* spell_owner, SpellTargetInfo* target_info) {
		target_info->target->buffs.push_back(new RunFast());
	}

	RunFast::RunFast() {
		remaining_duration = 5000;
		total_duration = 5000;
	}

	void RunFast::Apply(stats_t* stats, int* status_enable, int* status_disable) {
		stats->base_speed *= 2;
	}

	FootballPerson::FootballPerson() {
		target_type = TargetType::CHARACTER;

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