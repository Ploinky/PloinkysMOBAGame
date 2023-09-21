#include "person.h"
#include "game.h"
#include "missile.h"
#include "logger.h"

namespace PMG {
    void Person::Update(Game* game, float dt) {
        Attackable::Update(game, dt);

        // update cooldowns
        for (int i = 0; i < spells.size(); i++) {
            Spell* s = spells[i];
            if (s->remaining_cooldown != -1) {
                s->remaining_cooldown -= dt * 1000;

                if (s->remaining_cooldown <= 0) {
                    s->remaining_cooldown = -1;
                }
            }
        }
    }

    void Person::Act(Game* game, float dt) {
        if ((current_status & STATUS_STUNNED) == STATUS_STUNNED) {
            // we're stunned, we cannot do anything
            new_animation = "idle";
            return;
        }

        // ok, we get to actually decide what we want to do
        if (current_action_ != nullptr) {
            switch (current_action_->type) {
            case GameObjectActionType::STOP: {
                basic_attack_info.attack_started = false;
                spell_cast_info.current_spell = -1;
                new_animation = "idle";
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
                rotation.y = rotationY;

                if (target->team == team) {
                    // our teammate! do not attack! follow instead!
                    if (nav_agent.target.x != target->position.x || nav_agent.target.z != target->position.z) {
                        // start pathing!
                    }
                    MoveToward(target->position.x, target->position.z, game, 3);
                    new_animation = "run";
                    break;
                }

                if ((target->position - position).Length() > basic_attack_info.range) {
                    if (nav_agent.target.x != target->position.x || nav_agent.target.z != target->position.z) {
                        // start pathing!
                    }
                    // move towards target?
                    MoveToward(target->position.x, target->position.z, game, 3);

                    new_animation = "run";
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
                    target->TakeDamage(basic_attack_info.damage, this);
                }
                else {
                    Missile* basic_attack_missile = new Missile();
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


                new_animation = "idle";
                break;
            }
            case GameObjectActionType::MOVE: {
                basic_attack_info.attack_started = false;
                spell_cast_info.current_spell = -1;
                GameObjectActionMove* action = (GameObjectActionMove*)current_action_;
                MoveToward(action->target_point.x, action->target_point.y, game, 3);

                new_animation = "run";
                break;
            }
            case GameObjectActionType::CAST_SPELL: {

                new_animation = "idle";
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

                    if (spell->cast_animation.length() > 0) {
                        new_animation = spell->cast_animation;
                    }
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
    }

    void Person::Sync(std::vector<uint8_t>* data) {
        Attackable::Sync(data);

        // maybe not necessary to update on every tick? seems like a lot of work, but the accuracy does not need to be quite as high
        // maybe only update once cooldown is ready again?
        for (int i = 0; i < 4; i++) {
            Networking::CooldownPacket* pck = new Networking::CooldownPacket();
            pck->unit = unit_id;
            pck->spell_slot = i;
            pck->cooldown = spells[i]->remaining_cooldown;
            pck->total_cooldown = spells[i]->cooldown;
            pck->Write(data);
        }
    }
}