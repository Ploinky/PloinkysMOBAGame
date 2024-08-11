#include "Person.h"
#include <Game.h>
#include "Missile.h"
#include <Common/PMG_Common.h>
#include "Buff.h"

namespace PMG {
    void Person::Update(Client* game, float dt) {
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

        if (isDead_) {
            // currently waiting for respawn
            respawnTimer_ -= dt * 1000.0f;

            if (respawnTimer_ <= 0) {
                // TODO revive correctly?
                position = { 0, 0, 0 };
                stats.health = stats.max_health;
            }
            else {
                // TODO do we do nothing else?
                return;
            }
        }

        int status_enable = 0;
        int status_disable = 0;
        current_status = 0;

        modifiers = stats;
        for (Buff* buff : buffs) {
            buff->Update(dt);
            buff->Apply(&modifiers, &status_enable, &status_disable);
        }

        // now remove buffs that have run out
        std::erase_if(buffs, [](auto& kv) { return kv->should_remove; });

        current_status = status_enable;
        current_status &= (~status_disable);
    }

    void Person::Act(Client* game, float dt) {
        // TODO this goes to attackable?
        if ((current_status & STATUS_STUNNED) == STATUS_STUNNED) {
            // we're stunned, we cannot do anything
            return;
        }

        Attackable::Act(game, dt);
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
        // we do not despawn
        isDead_ = true;
        // TODO should probably not be hardcoded
        respawnTimer_ = 3000;
    }
}