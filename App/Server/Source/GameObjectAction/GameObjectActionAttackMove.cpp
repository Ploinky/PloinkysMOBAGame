#include <GameObject.h>
#include <Person.h>
#include <Missile.h>
#include <Game.h>

namespace PMG {
	void GameObjectActionAttackMove::Start() {
		Attackable* self = (Attackable*) game_->GetGameObjectById(selfId_);

        // we always rotate, no matter what happens!
        double rotationY = atan2(targetPoint_.x - self->position.x, targetPoint_.z - self->position.z) * 180.0f / M_PI;
        self->rotation.y = rotationY;
	}

	void GameObjectActionAttackMove::Do(float dt) {
		Attackable* self = (Attackable*) game_->GetGameObjectById(selfId_);

		Attackable* target = nullptr;
		// see if we find a target
		for(auto goIt : game_->igame_objects_) {
			IGameObject* go = goIt.second;

			if(go->unit_id == self->unit_id) {
				continue;
			}

			Attackable* newTarget = dynamic_cast<Attackable*>(go);

			if(newTarget == nullptr || newTarget->team == self->team) {
				continue;
			}
			
			if((newTarget->position - self->position).Length() <= self->basic_attack_info.acquisitionRange) {
				target = newTarget;
				break;
			}
		}

		if(target == nullptr) {
			// not targeting and nothing to target, so move
			self->MoveToward(targetPoint_.x, targetPoint_.z);
			return;
		}
		// we are currently trying to attack something!

  		if ((target->position - self->position).Length() > self->basic_attack_info.acquisitionRange) {
			// TODO if out of range, we forget about it and return to our path??
			target = nullptr;
		} else if((target->position - self->position).Length() > self->basic_attack_info.range) {
			// cannot attack target we found, so follow!
			self->MoveToward(target->position.x, target->position.z);
		} else {
			// TODO this should probably be in 1 place

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
				basic_attack_missile->missile_speed = 60;
				basic_attack_missile->damage = self->basic_attack_info.damage;
				basic_attack_missile->team = self->team;
				game_->SpawnMissile(basic_attack_missile);
			}

			// TODO what happens to the backswing?
			self->basic_attack_info.attack_started = false;
		}
	}

	void GameObjectActionAttackMove::Stop() {
	}
}