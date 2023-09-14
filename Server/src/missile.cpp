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
            target->TakeDamage(game, damage, owner);
            game->DestroyGameObject(this);
            return;
        }
        else if(scaled.Length() >= direction_vector.Length()) {
            game->DestroyGameObject(this);
            return;
        }

        // yikkkkeeessss
        if (target == nullptr) {
            for (auto& other_go_it : game->game_objects_) {
                GameObject* other_go = other_go_it.second;

                if (other_go->unit_id == this->unit_id || other_go->unit_id == owner->unit_id || !other_go->IsTargetable()) {
                    continue;
                }

                if (other_go->target_type == TargetType::BUILDING) {
                    continue;
                }

                Physics::Sphere other_hitbox = other_go->GetHitbox();

                if (Physics::TestCollision(GetHitbox(), other_hitbox)) {
                    game->ApplyDamage(other_go, damage);
                    game->DestroyGameObject(this);
                    return;
                }
            }
        }

        position = position + scaled;

        double rotationY = -atan2(target_current_position.z - position.z, target_current_position.x - position.x) * 180.0f / M_PI;
        Networking::UnitMovePacket* move = new Networking::UnitMovePacket();
        move->unit = unit_id;
        move->x = position.x;
        move->y = position.y;
        move->z = position.z;
        move->r = rotationY;
        game->SendPacket(move);
        return;
    }
}