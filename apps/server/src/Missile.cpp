#include "Missile.h"
#include "GameObject.h"

/*
void Missile::Act(Client* game, float dt) {
    Vector3 current_position = position;

    Vector3 target_current_position = this->target_point;

    if (target != nullptr) {
        target_current_position = this->target->position;
    }

    Vector3 direction_vector = target_current_position - current_position;
    Vector3 scaled = direction_vector.ScaleToLength(missile_speed);
    scaled = scaled * dt;

    if (target != nullptr && scaled.Length() >= direction_vector.Length()) {
        target->TakeDamage(damage, owner);
        Destroy();
        return;
    }
    else if(scaled.Length() >= direction_vector.Length()) {
        Destroy();
        return;
    }

    // yikkkkeeessss
    if (target == nullptr) {
        for (auto& other_go_it : game->GameState.GameObjects) {
            CGameObject* other_go = other_go_it.second;

            if (other_go->unit_id == this->unit_id || other_go->unit_id == owner->unit_id) {
                continue;
            }

            if (Attackable* other = dynamic_cast<Attackable*>(other_go)) {
                if (other->target_type == TargetType::BUILDING) {
                    continue;
                }

                Sphere other_hitbox = Sphere(other->position, other->collision_radius);

                if (TestCollision(GetHitbox(), other_hitbox)) {
                    other->TakeDamage(damage, this);
                    Destroy();
                    return;
                }
            }
        }
    }

    position = position + scaled;

    rotation.y = -atan2(target_current_position.z - position.z, target_current_position.x - position.x) * 180.0f / M_PI;
}
*/
