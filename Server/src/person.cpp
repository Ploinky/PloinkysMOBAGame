#include "person.h"
#include "game.h"
#include "missile.h"
#include "logger.h"

namespace PMG {
	void Person::Act(Game* game, float dt) {
        if ((current_status & STATUS_STUNNED) == STATUS_STUNNED) {
            return;
        }

        if (current_action_ != nullptr) {
            switch (current_action_->type) {
            case GameObjectActionType::STOP: {
                basic_attack_info.attack_started = false;
                spell_cast_info.current_spell = -1;
                break;
            }
            case GameObjectActionType::ATTACK_UNIT: {
                GameObjectActionAttackUnit* action = (GameObjectActionAttackUnit*)current_action_;
                Attackable* target = dynamic_cast<Attackable*>(game->GetGameObjectById(action->target_net_id));

                if (target == nullptr || target->unit_id == unit_id) {
                    // nothing to attack?
                    this->current_action_ = new GameObjectActionStop();
                    break;
                }

                // we always rotate, no matter what happens!
                double rotationY = atan2(target->position.x - position.x, target->position.z - position.z) * 180.0f / M_PI;
                Networking::UnitMovePacket* move = new Networking::UnitMovePacket();
                move->unit = unit_id;
                move->x = position.x;
                move->y = position.y;
                move->z = position.z;
                move->r = rotationY;
                game->SendPacket(move);

                if (target->team == team) {
                    // our teammate! do not attack! follow instead!
                    if (nav_agent.target.x != target->position.x || nav_agent.target.z != target->position.z) {
                        // start pathing!
                    }
                    MoveToward(target->position.x, target->position.z, game, 3);
                    break;
                }

                if ((target->position - position).Length() > basic_attack_info.range) {
                    if (nav_agent.target.x != target->position.x || nav_agent.target.z != target->position.z) {
                        // start pathing!
                    }
                    // move towards target?
                    MoveToward(target->position.x, target->position.z, game, 3);
                    break;
                }

                // we're in range, check if we can attack
                unsigned long long ticks_since = game->gameTick - basic_attack_info.last_attack;

                // how many ms do we wait after 1 attack
                double ms_per_attack = 1000.0 / basic_attack_info.attack_speed;

                if (ticks_since * TICKRATE < ms_per_attack) {
                    // cannot attack again yet
                    break;
                }

                // we can attack, wtf to do now?!
                // consider forward- and backswing as well, yikes
                if (!basic_attack_info.attack_started) {
                    basic_attack_info.attack_started_at = game->gameTick;
                    basic_attack_info.attack_started = TRUE;
                    // ok we start... do we also need to let someone know? :O
                    break;
                }

                ticks_since = game->gameTick - basic_attack_info.attack_started_at;
                double ms_until_hit = ms_per_attack * basic_attack_info.hit_point;

                if (ticks_since * TICKRATE < ms_until_hit) {
                    // still swinging!
                    break;
                }

                // attack triggered!
                if (basic_attack_info.type == MELEE) {
                    target->stats.health -= basic_attack_info.damage;

                    if (target->stats.health < 0) {
                        target->stats.health = 0;
                    }

                    Networking::UnitStatsPacket* stats = new Networking::UnitStatsPacket();
                    stats->unit = target->unit_id;
                    stats->health = target->stats.health;
                    stats->max_health = target->stats.max_health;
                    game->SendPacket(stats);
                }
                else {
                    Missile* basic_attack_missile = new Missile();
                    basic_attack_missile->unit_id = game->current_entity_id_++;
                    // basic_attack_missile->owner = this;
                    basic_attack_missile->target = target;
                    basic_attack_missile->position = position;
                    basic_attack_missile->missile_speed = 60;
                    basic_attack_missile->damage = basic_attack_info.damage;
                    basic_attack_missile->team = team;
                    game->SpawnMissile(basic_attack_missile);
                }

                basic_attack_info.last_attack = game->gameTick;
                basic_attack_info.attack_started = FALSE;
                break;
            }
            case GameObjectActionType::MOVE: {
                basic_attack_info.attack_started = false;
                spell_cast_info.current_spell = -1;
                GameObjectActionMove* action = (GameObjectActionMove*)current_action_;
                MoveToward(action->target_point.x, action->target_point.y, game, 3);
                break;
            }
            case GameObjectActionType::CAST_SPELL: {
                GameObjectActionCastSpell* action = (GameObjectActionCastSpell*)current_action_;

                if (action->spell_index < 0 || action->spell_index >= spells.size()) {
                    // cannot cast spell that does not exist
                    Logger::Err("Error: Attempt to cast spell that does not exist.");
                    current_action_ = new GameObjectActionStop();
                    break;
                }

                Spell* spell = spells[action->spell_index];

                // no spell being cast yet
                if (spell_cast_info.current_spell == -1) {
                    if (spell->remaining_cooldown != -1) {
                        break;
                    }

                    spell_cast_info.current_spell = action->spell_index;
                    spell_cast_info.cast_time = game->gameTick;
                    // TODO send packet to let clients know what's happening?

                    spell->CastStart(game, this, action->target_info);
                }

                if ((game->gameTick - spell_cast_info.cast_time) * TICKRATE > spell->cast_point) {
                    // spell cast successfully
                    // wtf now?
                    spell_cast_info.current_spell = -1;
                    spell_cast_info.cast_time = 0;

                    spell->Cast(game, this, action->target_info);

                    spell->remaining_cooldown = spell->cooldown;
                    current_action_ = new GameObjectActionStop();

                    Networking::CooldownPacket* pck = new Networking::CooldownPacket();
                    pck->unit = unit_id;
                    pck->spell_slot = action->spell_index;
                    pck->cooldown = spell->remaining_cooldown;
                    pck->total_cooldown = spell->cooldown;
                    game->SendPacket(pck);
                    break;
                }
            }
            }
        }
	};


    void Person::MoveToward(double x, double z, Game* game, double move_speed) {
        nav_agent_t navAgent = nav_agent;
        navAgent.target.x = x;
        navAgent.target.z = z;

        if (Physics::CompareFloat(navAgent.target.x, position.x) && Physics::CompareFloat(navAgent.target.z, position.z)) {
            current_action_ = new GameObjectActionStop();
            // Already at target
            return;
        }

        if (navAgent.path.empty()) {

            // No path to follow, we need a new path!
            navAgent.path = game->m_navMesh->PlanPath({ static_cast<float>(position.x), 0, static_cast<float>(position.z) }, navAgent.target);

            // New path is empty, we are requesting an invalid path
            if (navAgent.path.empty()) {
                return;
            }
            // First is our start point? yikes.
            navAgent.path.pop_front();
        }

        vertex_t intermediateTarget = navAgent.path.front();

        if (abs(position.x - intermediateTarget.x) < 0.001 && abs(position.z - intermediateTarget.z) < 0.001) {
            // Next frame we follow next?!
            navAgent.path.pop_front();

            if (!navAgent.path.empty()) {
                intermediateTarget = navAgent.path.front();
            }
        }

        float tx = intermediateTarget.x;
        float ty = intermediateTarget.z;

        if (Physics::CompareFloat(position.x, tx) && Physics::CompareFloat(position.z, ty)) {
            return;
        }

        float dx = tx - position.x;
        float dy = ty - position.z;
        float length = sqrt(dx * dx + dy * dy);


        dx /= length;
        dy /= length;

        float newX = position.x + move_speed * dx * TICKRATE / 1000.0f;
        float newY = position.z + move_speed * dy * TICKRATE / 1000.0f;

        position.x = (position.x < tx && newX >= tx) || (position.x > tx && newX <= tx) ? tx : newX;
        position.z = (position.z < ty && newY >= ty) || (position.z > ty && newY <= ty) ? ty : newY;

        if (position.x != tx || position.z != ty) {
            rotation.y = atan2(tx - position.x, ty - position.z) * 180.0f / M_PI;
        }

        Networking::UnitMovePacket* move = new Networking::UnitMovePacket();
        move->unit = unit_id;
        move->x = position.x;
        move->y = position.y;
        move->z = position.z;
        move->r = rotation.y;
        game->SendPacket(move);
    }
}