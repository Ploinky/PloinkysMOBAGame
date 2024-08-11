#include <GameObject.h>
#include <Person.h>
#include <Missile.h>
#include <Game.h>

namespace PMG {
	void GameObjectActionAttackUnit::Start() {
		Attackable* target = (Attackable*) game_->GetGameObjectById(targetId_);
		Attackable* self = (Attackable*) game_->GetGameObjectById(selfId_);

        if (target == nullptr || target->unit_id == selfId_) {
            // nothing to attack?
            self->DoAction(new GameObjectActionStop(self, game_));
            return;
        }

        // we always rotate, no matter what happens!
        double rotationY = atan2(target->position.x - self->position.x, target->position.z - self->position.z) * 180.0f / M_PI;
        self->rotation.y = rotationY;
	}

	void GameObjectActionAttackUnit::Do(float dt) {
		Attackable* target = (Attackable*) game_->GetGameObjectById(targetId_);
		Attackable* self = (Attackable*) game_->GetGameObjectById(selfId_);

		if(target == nullptr) {
			// nothing to attack?
			self->DoAction(new GameObjectActionStop(self, game_));
			return;
		}

        if (target->team == self->team) {
            // our teammate! do not attack! follow instead!
            if (self->nav_agent.target.x != target->position.x || self->nav_agent.target.z != target->position.z) {
                // start pathing!
                self->StartMove(target->position.x, target->position.z);
            }
            self->MoveToward(target->position.x, target->position.z);

            return;
        }

        if ((target->position - self->position).Length() > self->basic_attack_info.range) {
            if (self->nav_agent.target.x != target->position.x || self->nav_agent.target.z != target->position.z) {
                // start pathing!
                self->StartMove(target->position.x, target->position.z);
            }
            // move towards target?
            self->MoveToward(target->position.x, target->position.z);

            return;
        }

        // we're in range, check if we can attack
        // how many ms do we wait after 1 attack
        double ms_per_attack = 1000.0 / self->basic_attack_info.attack_speed;

        if ((self->basic_attack_info.sinceAttack * 1000.0f) < ms_per_attack) {
            // cannot attack again yet
            return;
        }

        // we can attack, wtf to do now?!
        // consider forward- and backswing as well, yikes
        if (!self->basic_attack_info.attack_started) {
            self->basic_attack_info.sinceAttack = 0;
            self->basic_attack_info.sinceAttackStarted = 0;
            self->basic_attack_info.attack_started = true;
            // ok we start... do we also need to let someone know? :O
            return;
        }

        self->basic_attack_info.sinceAttackStarted += dt;
        double ms_until_hit = ms_per_attack * self->basic_attack_info.hit_point;

        if ((self->basic_attack_info.sinceAttackStarted * 1000.0f) < ms_until_hit) {
            // still swinging!
            return;
        }

        // attack triggered!
        if (self->basic_attack_info.type == MELEE) {
            target->TakeDamage(self->basic_attack_info.damage, self);
        }
        else {
            Missile* basic_attack_missile = new Missile();
            basic_attack_missile->owner = self;
            basic_attack_missile->target = target;
            basic_attack_missile->position = self->position;
            basic_attack_missile->missile_speed = 6000;
            basic_attack_missile->damage = self->basic_attack_info.damage;
            basic_attack_missile->team = self->team;
            game_->SpawnMissile(basic_attack_missile);
        }

        // TODO what happens to the backswing?
        self->basic_attack_info.attack_started = false;
	}

	void GameObjectActionAttackUnit::Stop() {
	}
}