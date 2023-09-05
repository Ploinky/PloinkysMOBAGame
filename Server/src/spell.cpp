
#include "spell.h"
#include "character.h"
#include "game.h"
#include "missile.h"

namespace PMG {
	void TestSpell::OnCast(Game* game, Character* spell_owner, Physics::Vector3 target_point) {
		Missile* missile = new Missile();
		missile->damage = 10;
		missile->missile_speed = 30;
		missile->team = spell_owner->team;
		missile->position = spell_owner->position;
		missile->target = nullptr;
		missile->target_point = target_point;
		missile->max_distance = 10;
		missile->owner = spell_owner;
		missile->position.y = 1;
		missile->target_point.y = 1;
		game->SpawnMissile(missile);
	}
	
	TestSpell::TestSpell() {
		cast_point = 10;
		cooldown = 1000;
		type = SpellType::MISSILE;
	}
}