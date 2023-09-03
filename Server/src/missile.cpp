#include "missile.h"

namespace PMG {
	void Missile::Think(float dt, Game* game) {
        Physics::Vector3 current_position = position;

        if (target == nullptr) {
            // wth?
            return;
        }

        Physics::Vector3 target_current_position = target->position;

        Physics::Vector3 direction_vector = target_current_position - current_position;
        Physics::Vector3 scaled = direction_vector.ScaleToLength(missile_speed);
        scaled = scaled * dt;

        if (scaled.Length() >= direction_vector.Length()) {
            // we hit?!
            target->stats.health -= damage;

            if (target->stats.health < 0) {
                target->stats.health = 0;
            }

            game->SendPacket<pck_unit_stats_t>(PacketType::PCK_STATS, { target->unit_id, target->stats.health, target->stats.max_health });
            game->SendPacket<pck_unit_despawn_t>(PacketType::UNITDESPAWN, { unit_id });

            is_destroyed = true;
            return;
        }

        position = position + scaled;
        game->SendPacket<pck_unit_move_t>(PacketType::UNITMOVE, { unit_id, static_cast<float>(position.x), static_cast<float>(position.y), static_cast<float>(rotation.y) });
        return;
    }
}