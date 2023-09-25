#include "person.h"
#include "game.h"
#include "missile.h"
#include "logger.h"
#include "buff.h"

namespace PMG {
    void Person::Update(Game* game, float dt) {
        Attackable::Update(game, dt);

        stats_t frame_stats;
        int status_enable = 0;
        int status_disable = 0;
        current_status = 0;

        for (Buff* buff : buffs) {
            buff->Update(dt);
            buff->Apply(&frame_stats, &status_enable, &status_disable);
        }

        // now remove buffs that have run out
        std::erase_if(buffs, [](auto& kv) { return kv->should_remove; });

        current_status = status_enable;
        current_status &= (~status_disable);

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
            new_animation = "stunned";
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

                    new_animation = "attack1";
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

    void Person::Die() {
        // we do not die!!!
        position = { 0, 0, 0 };
        stats.health = stats.max_health;
    }
}