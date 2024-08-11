#include "Attackable.h"
#include "Common/pmg_networking.h"
#include "Game.h"

namespace PMG {
	void Attackable::Update(float dt) {
		basic_attack_info.sinceAttack += dt;

		modifiers = stats;

		if (stats.health == stats.max_health) {
			// TODO this seems a bit harsh
			return;
		}
		
		stats.health += stats.health_regen * (dt / (1000.0f / 60.0f));

		if (stats.health > stats.max_health) {
			stats.health = stats.max_health;
		}

		stats_updated = true;
	}

	void Attackable::TakeDamage(float damage, IGameObject* source) {
		stats.health -= damage;

		if (stats.health <= 0) {
			// TODO die?
			stats.health = 0;
			Die();
		}

		stats_updated = true;
	}

	void Attackable::Heal(float heal, IGameObject* source) {
		if (stats.health = stats.max_health) {
			return;
		}

		stats.health += heal;

		if (stats.health > stats.max_health) {
			stats.health = stats.max_health;
		}

		stats_updated = true;
	}

	void Attackable::DoAction(GameObjectAction* newAction) {
		if (current_action_ != nullptr) {
			current_action_->Stop();
			delete current_action_;
			current_action_ = nullptr;
		}

		if (newAction != nullptr) {
			current_action_ = newAction;
			current_action_->Start();
		}
	}

	void Attackable::Act(Client* game, float dt) {
		if (current_action_ != nullptr) {
			current_action_->Do(dt);
		}
	}

	void Attackable::StartMove(float x, float z) {
		nav_agent.path.clear();

		nav_agent.target.x = x;
		nav_agent.target.z = z;

		// No path to follow, we need a new path!
		nav_agent.path = nav_agent.grid->GetPath({ static_cast<float>(position.x), static_cast<float>(position.z) }, { nav_agent.target.x, nav_agent.target.z });

		// New path is empty, we are requesting an invalid path
		if (nav_agent.path.empty()) {
			return;
		}
	}

	void Attackable::MoveToward(double x, double z) {
		if(nav_agent.target.x != x || nav_agent.target.z != z) {
			StartMove(x, z);
		}

		if (Physics::CompareFloat(nav_agent.target.x, position.x) && Physics::CompareFloat(nav_agent.target.z, position.z)) {
			DoAction(new GameObjectActionStop(this, nullptr));
			// Already at target
			return;
		}

		if (nav_agent.path.empty()) {
			// No path to follow, we need a new path!
			nav_agent.path = nav_agent.grid->GetPath({ static_cast<float>(position.x), static_cast<float>(position.z) }, { nav_agent.target.x, nav_agent.target.z }, true);

			// New path is empty, we are requesting an invalid path
			if (nav_agent.path.empty()) {
				return;
			}
		}

		Physics::Vector2 intermediateTarget = nav_agent.path.front();

		if (abs(position.x - intermediateTarget.x) < 0.001 && abs(position.z - intermediateTarget.y) < 0.001) {
			// Next frame we follow next?!
			nav_agent.path.erase(nav_agent.path.begin());

			if (!nav_agent.path.empty()) {
				intermediateTarget = nav_agent.path.front();
			}
		}

		float tx = intermediateTarget.x;
		float ty = intermediateTarget.y;

		if (Physics::CompareFloat(position.x, tx) && Physics::CompareFloat(position.z, ty)) {
			return;
		}

		float dx = tx - position.x;
		float dy = ty - position.z;
		float length = sqrt(dx * dx + dy * dy);


		dx /= length;
		dy /= length;

		float newX = position.x + modifiers.move_speed * dx * TICKRATE / 1000.0f;
		float newY = position.z + modifiers.move_speed * dy * TICKRATE / 1000.0f;

		NavigationCell* currentCell = nav_agent.grid->GetCellAt(position.x, position.z);
		NavigationCell* nextCell = nav_agent.grid->GetCellAt(newX, newY);

		if(currentCell != nextCell) {
			if(!nextCell->IsOpen || !nextCell->IsWalkable) {
				// running into occupied cell, renav next frame!
				nav_agent.path = {};
				// person_->nav_agent.grid->GetPath({ static_cast<float>(person_->position.x), static_cast<float>(person_->position.z) }, { person_->nav_agent.target.x, person_->nav_agent.target.z });
				// quit because we do not actually move now
				return;
			}
		}

		position.x = (position.x < tx && newX >= tx) || (position.x > tx && newX <= tx) ? tx : newX;
		position.z = (position.z < ty && newY >= ty) || (position.z > ty && newY <= ty) ? ty : newY;

		if (position.x != tx || position.z != ty) {
			rotation.y = atan2(tx - position.x, ty - position.z) * 180.0f / static_cast<float>(M_PI);
		}
	}

	void Attackable::Die() {
		is_destroyed = true;
	}

	void Attackable::OnCollision(Client* game, IGameObject* o) {
	}

	void Attackable::Sync(std::vector<uint8_t>* data) {
		if (!spawn_synced && !is_destroyed) {
			Networking::SpawnPacket pck = Networking::SpawnPacket();
			pck.unit = unit_id;
			pck.team = team;
			pck.unit_type = prefab;
			pck.x = position.x;
			pck.y = position.y;
			pck.z = position.z;

			pck.Write(data);
			spawn_synced = true;
		}

		if (spawn_synced && is_destroyed) {
			Networking::DespawnPacket pck = Networking::DespawnPacket();
			pck.unit = unit_id;

			pck.Write(data);
			spawn_synced = false;
		}

		if (stats_updated) {
			Networking::UnitStatsPacket pck = Networking::UnitStatsPacket();
			pck.unit = unit_id;
			pck.health = stats.health;
			pck.max_health = stats.max_health;

			pck.Write(data);

			stats_updated = false;
		}

		if(current_action_ == nullptr || current_action_->type == GameObjectActionType::STOP) {
			Networking::UnitIdlePacket idle = Networking::UnitIdlePacket();
			idle.unit = unit_id;
			idle.x = position.x;
			idle.y = position.y;
			idle.z = position.z;
			idle.r = rotation.y;
			idle.Write(data);
		} else if (current_action_->type == GameObjectActionType::ATTACK_UNIT && basic_attack_info.attack_started) {
			Networking::AttackStartPacket attack = Networking::AttackStartPacket();
			attack.content.unit = unit_id;
			attack.content.target = ((GameObjectActionAttackUnit*)current_action_)->targetId_;
			attack.Write(data);
		} else {
			Networking::UnitMovePacket move = Networking::UnitMovePacket();
			move.unit = unit_id;
			move.x = position.x;
			move.y = position.y;
			move.z = position.z;
			move.r = rotation.y;
			move.Write(data);
		}
	}
}