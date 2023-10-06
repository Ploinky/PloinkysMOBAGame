#include "attackable.h"
#include "pmg_networking.h"
#include "game.h"

namespace PMG {
	void Attackable::Update(float dt) {
		modifiers = stats;

		if (stats.health == stats.max_health) {
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

	void Attackable::MoveToward(double x, double z, Client* game) {
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

		float newX = position.x + modifiers.move_speed * dx * TICKRATE / 1000.0f;
		float newY = position.z + modifiers.move_speed * dy * TICKRATE / 1000.0f;

		position.x = (position.x < tx && newX >= tx) || (position.x > tx && newX <= tx) ? tx : newX;
		position.z = (position.z < ty && newY >= ty) || (position.z > ty && newY <= ty) ? ty : newY;

		if (position.x != tx || position.z != ty) {
			rotation.y = atan2(tx - position.x, ty - position.z) * 180.0f / M_PI;
		}

	}

	void Attackable::Die() {
		is_destroyed = true;
	}

	void Attackable::OnCollision(Client* game, IGameObject* o) {
		Attackable* other = dynamic_cast<Attackable*>(o);

		if (other == nullptr || target_type == TargetType::BUILDING || (current_action_ != nullptr && current_action_->type != GameObjectActionType::MOVE)) {
			// only collide with other attackables
			return;
		}

		// find direction in which to push
		Physics::Vector3 dir = position - o->position;
		float dist = dir.Length();
		dir = dir.Normalize();
		
		// find distance to push
		dir = dir.ScaleToLength(o->collision_radius - dist);

		// push
		position = position + dir;
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

		if (new_animation.length() > 0) {
			Networking::AnimationPacket* pck = new Networking::AnimationPacket();
			pck->unit_id = unit_id;
			pck->animation_name = new_animation;
			pck->loop = new_animation.compare("idle") == 0;
			pck->Write(data);

			new_animation = "";
		}

		Networking::UnitMovePacket* move = new Networking::UnitMovePacket();
		move->unit = unit_id;
		move->x = position.x;
		move->y = position.y;
		move->z = position.z;
		move->r = rotation.y;
		move->Write(data);
	}
}