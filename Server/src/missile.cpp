#include "missile.h"
#include "attackable.h"

namespace PMG {
	void Missile::Act(Game* game, float dt) {
        Physics::Vector3 current_position = position;

        Physics::Vector3 target_current_position = this->target_point;

        if (target != nullptr) {
            target_current_position = this->target->position;
        }

        Physics::Vector3 direction_vector = target_current_position - current_position;
        Physics::Vector3 scaled = direction_vector.ScaleToLength(missile_speed);
        scaled = scaled * dt;

        if (target != nullptr && scaled.Length() >= direction_vector.Length()) {
            target->TakeDamage(damage, owner);
            is_destroyed = true;
            return;
        }
        else if(scaled.Length() >= direction_vector.Length()) {
            is_destroyed = true;
            return;
        }

        // yikkkkeeessss
        if (target == nullptr) {
            for (auto& other_go_it : game->igame_objects_) {
                IGameObject* other_go = other_go_it.second;

                if (other_go->unit_id == this->unit_id || other_go->unit_id == owner->unit_id) {
                    continue;
                }

                if (Attackable* other = dynamic_cast<Attackable*>(other_go)) {
                    if (other->target_type == TargetType::BUILDING) {
                        continue;
                    }

                    Physics::Sphere other_hitbox = Physics::Sphere(other->position, other->collision_radius);

                    if (Physics::TestCollision(GetHitbox(), other_hitbox)) {
                        other->TakeDamage(damage, this);
                        is_destroyed = true;
                        return;
                    }
                }
            }
        }

        position = position + scaled;

        rotation.y = -atan2(target_current_position.z - position.z, target_current_position.x - position.x) * 180.0f / M_PI;
    }

    void Missile::Sync(std::vector<uint8_t>* data) {
        if (!is_destroyed && !spawn_synced) {
            Networking::SpawnPacket pck = Networking::SpawnPacket();
            pck.unit = unit_id;
            pck.team = team;
            pck.unit_type = UnitPrefab::THROW_FOOTBALL;
            pck.x = position.x;
            pck.y = position.y;
            pck.z = position.z;

            pck.Write(data);
            spawn_synced = true;
        }

        if (is_destroyed && spawn_synced) {
            Networking::DespawnPacket pck = Networking::DespawnPacket();
            pck.unit = unit_id;

            pck.Write(data);
            spawn_synced = false;
        }

        Networking::UnitMovePacket* move = new Networking::UnitMovePacket();
        move->unit = unit_id;
        move->x = position.x;
        move->y = position.y;
        move->z = position.z;
        move->r = rotation.y;
        move->Write(data);
    };
}