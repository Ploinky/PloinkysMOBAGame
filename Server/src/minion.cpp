#include "minion.h"
#include "game.h"
#include "missile.h"

namespace PMG {
	AttackableStats minion_attackable_stats = {
        3, // move_speed
		10,
		10,
		0,
		1,
		0,
	};

	void Minion::Update(Game* game, float dt) {
        Attackable::Update(game, dt);

        if (current_action_ != nullptr && current_action_->type == GameObjectActionType::ATTACK_UNIT) {
            GameObjectActionAttackUnit* action = (GameObjectActionAttackUnit*)current_action_;
            Attackable* target = (Attackable*)game->GetGameObjectById(action->target_net_id);

            if (target == nullptr || target->unit_id == unit_id) {
                // nothing to attack?
                this->current_action_ = new GameObjectActionStop();
                return;
            }


            if ((target->position - position).Length() > basic_attack_info.range) {
                MoveToward(target->position.x, target->position.z, game);
                return;
            }

            // we're in range, check if we can attack
            unsigned long long ticks_since = game->gameTick - basic_attack_info.last_attack;

            // how many ms do we wait after 1 attack
            double ms_per_attack = 1000.0 / basic_attack_info.attack_speed;

            if (ticks_since * TICKRATE < ms_per_attack) {
                // cannot attack again yet
                return;
            }

            // we can attack, wtf to do now?!
            // consider forward- and backswing as well, yikes
            if (!basic_attack_info.attack_started) {
                basic_attack_info.attack_started_at = game->gameTick;
                basic_attack_info.attack_started = TRUE;
                // ok we start... do we also need to let someone know? :O
                return;
            }

            ticks_since = game->gameTick - basic_attack_info.attack_started_at;
            double ms_until_hit = ms_per_attack * basic_attack_info.hit_point;

            if (ticks_since * TICKRATE < ms_until_hit) {
                // still swinging!
                return;
            }

            // attack triggered!
            if (basic_attack_info.type == MELEE) {
                target->TakeDamage(basic_attack_info.damage, this);
            }
            else {
                Missile* basic_attack_missile = new Missile();
                basic_attack_missile->unit_id = game->current_entity_id_++;
                basic_attack_missile->owner = this;
                basic_attack_missile->target = target;
                basic_attack_missile->position = position;
                basic_attack_missile->missile_speed = 60;
                basic_attack_missile->damage = basic_attack_info.damage;
                basic_attack_missile->team = team;
                game->SpawnMissile(basic_attack_missile);
            }

            basic_attack_info.last_attack = game->gameTick;
            basic_attack_info.attack_started = FALSE;
            return;
        }

        // try to find unit to attack
        for (auto go_it : game->igame_objects_) {
            IGameObject* other_go = go_it.second;

            if (other_go == nullptr || other_go->unit_id == unit_id) {
                continue;
            }

            if (Attackable* other = dynamic_cast<Attackable*>(other_go)) {
                if (other->target_type == TargetType::UNTARGETABLE) {
                    continue;
                }

                if (other->team == team) {
                    continue;
                }

                double dist = (other->position - position).Length();

                if (dist <= 5) {
                    current_action_ = new GameObjectActionAttackUnit(other_go->unit_id);
                    return;
                }
            }
        }

        MoveToward(waypoints_[0].x, waypoints_[0].z, game);
	};
}