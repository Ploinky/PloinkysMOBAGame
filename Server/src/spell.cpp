
#include "spell.h"
#include "character.h"
#include "game.h"

namespace PMG {
	void TestSpell::TargetHit(Game* game, Character* target) {
		target->stats.health -= 50;
		game->SendPacket<pck_unit_stats_t>(PacketType::PCK_STATS, { target->unit_id, target->stats.health, target->stats.max_health });
	}
	
	TestSpell::TestSpell() {
		cast_point = 500;
		type = SpellType::MISSILE;
	}
}