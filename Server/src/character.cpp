#include "character.h"
#include "game.h"
#include "missile.h"
#include "logger.h"

namespace PMG {
	void Character::Think(float dt, Game* game) {
        for (int i = 0; i < spells.size(); i++) {
            Spell* s = spells[i];
            if (s->remaining_cooldown != -1) {
                s->remaining_cooldown -= dt * 1000;

                if (s->remaining_cooldown <= 0) {
                    s->remaining_cooldown = -1;
                    game->SendPacket<pck_spell_cooldown_t>(PacketType::PCK_SPELL_COOLDOWN, { this->unit_id, i, s->remaining_cooldown });
                }
            }
        }

        if (current_action != nullptr) {
            switch (current_action->type) {
            case CharacterActionType::STOP: {
                basic_attack_info.attack_started = false;
                spell_cast_info.current_spell = -1;
                break;
            }
            case CharacterActionType::ATTACK_UNIT: {
                CharacterActionAttackUnit* action = (CharacterActionAttackUnit*)current_action;
                Character* target = (Character*)game->GetGameObjectById(action->target_net_id);

                if (target == nullptr || target->unit_id == unit_id) {
                    // nothing to attack?
                    this->current_action = new CharacterActionStop();
                    break;
                }

                if ((target->position - position).Length() > basic_attack_info.range) {
                    // move towards target?
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

                    game->SendPacket<pck_unit_stats_t>(PacketType::PCK_STATS, { target->unit_id, target->stats.health, target->stats.max_health });
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
                break;
            }
            case CharacterActionType::MOVE: {
                basic_attack_info.attack_started = false;
                spell_cast_info.current_spell = -1;
                CharacterActionMove* action = (CharacterActionMove*)current_action;

                // figure out if we're already going to target

                // ======== Navigation system ========
                nav_agent_t navAgent = nav_agent;
                navAgent.target.x = action->target_point.x;
                navAgent.target.z = action->target_point.y;

                if (navAgent.target.x == position.x && navAgent.target.z == position.y) {
                    // Already at target
                    break;
                }

                if (navAgent.path.empty()) {
                    // No path to follow, we need a new path!
                    navAgent.path = game->m_navMesh->PlanPath({ static_cast<float>(position.x), 0, static_cast<float>(position.z) }, navAgent.target);

                    // New path is empty, we are requesting an invalid path
                    if (navAgent.path.empty()) {
                        break;
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

                if (Physics::CompareDouble(position.x, tx) && Physics::CompareDouble(position.z, ty)) {
                    break;
                }

                float dx = tx - position.x;
                float dy = ty - position.z;
                float length = sqrt(dx * dx + dy * dy);


                dx /= length;
                dy /= length;

                float newX = position.x + 3.0f * dx * TICKRATE / 1000.0f;
                float newY = position.z + 3.0f * dy * TICKRATE / 1000.0f;

                position.x = (position.x < tx && newX >= tx) || (position.x > tx && newX <= tx) ? tx : newX;
                position.z = (position.z < ty && newY >= ty) || (position.z > ty && newY <= ty) ? ty : newY;

                if (position.x != tx || position.z != ty) {
                    rotation.y = -atan2(ty - position.z, tx - position.x) * 180.0f / M_PI;
                }

                game->SendPacket<pck_unit_move_t>(PacketType::UNITMOVE, { unit_id, position.x, position.y, position.z, static_cast<float>(rotation.y) });
                break;
            }
            case CharacterActionType::CAST_SPELL: {
                CharacterActionCastSpell* action = (CharacterActionCastSpell*)current_action;
                
                if (action->spell_index < 0 || action->spell_index >= spells.size()) {
                    // cannot cast spell that does not exist
                    Logger::Err("Error: Attempt to cast spell that does not exist.");
                    current_action = new CharacterActionStop();
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
                    // TODO send packet to let clients know what's happening

                    rotation.y = -atan2(action->target_point.z - position.z, action->target_point.x - position.x) * 180.0f / M_PI;
                    game->SendPacket<pck_unit_move_t>(PacketType::UNITMOVE, { unit_id, position.x, position.y, position.z, rotation.y });
                }

                if ((game->gameTick - spell_cast_info.cast_time) * TICKRATE > spell->cast_point) {
                    // spell cast successfully
                    // wtf now?
                    spell_cast_info.current_spell = -1;
                    spell_cast_info.cast_time = 0;
                    spell->OnCast(game, this, action->target_point);
                    spell->remaining_cooldown = spell->cooldown;
                    current_action = new CharacterActionStop();
                    game->SendPacket<pck_spell_cooldown_t>(PacketType::PCK_SPELL_COOLDOWN, { this->unit_id, action->spell_index, spell->cooldown });
                    break;
                }
            }
            }
        }
	}
}