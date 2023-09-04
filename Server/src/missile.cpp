#include "missile.h"

namespace PMG {
	void Missile::Think(float dt, Game* game) {
        Physics::Vector3 current_position = position;

        Physics::Vector3 target_current_position = this->target_point;

        if (target != nullptr) {
            target_current_position = this->target->position;
        }

        Physics::Vector3 direction_vector = target_current_position - current_position;
        Physics::Vector3 scaled = direction_vector.ScaleToLength(missile_speed);
        scaled = scaled * dt;

        if (target != nullptr && scaled.Length() >= direction_vector.Length()) {
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
        else if(scaled.Length() >= direction_vector.Length()) {
            // is this enough to be non targeted? probably not?
            game->SendPacket<pck_unit_despawn_t>(PacketType::UNITDESPAWN, { unit_id });
            is_destroyed = true;
            return;
        }

        // yikkkkeeessss
        if (target == nullptr) {
            for (auto& other_go_it : game->game_objects_) {
                GameObject* other_go = other_go_it.second;

                if (other_go->unit_id == this->unit_id || other_go->unit_id == owner->unit_id || !other_go->IsTargetable()) {
                    continue;
                }

                Physics::Sphere other_hitbox = other_go->GetHitbox();

                if (Physics::TestCollision(GetHitbox(), other_hitbox)) {
                    Character* charc = (Character*)other_go;
                    charc->stats.health -= damage;

                    if (charc->stats.health < 0) {
                        charc->stats.health = 0;
                    }

                    game->SendPacket<pck_unit_stats_t>(PacketType::PCK_STATS, { charc->unit_id, charc->stats.health, charc->stats.max_health });
                    game->SendPacket<pck_unit_despawn_t>(PacketType::UNITDESPAWN, { unit_id });

                    is_destroyed = true;
                    return;
                }
            }
        }

        position = position + scaled;
        game->SendPacket<pck_unit_move_t>(PacketType::UNITMOVE, { unit_id, position.x, position.y, position.z, static_cast<float>(rotation.y) });
        return;
    }
}