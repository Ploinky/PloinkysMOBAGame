#include "GameObject.h"
#include <Game.h>
#include "Missile.h"
#include <Common/PMG_Common.h>
#include "Buff.h"
#include "Spell.h"
#include <GameObject/CGameObject.h>

int STATUS_STUNNED = 0b1;

void CGameObject::StartMove(float x, float z) {
	nav_agent.path.clear();

	nav_agent.target.x = x;
	nav_agent.target.z = z;

	// No path to follow, we need a new path!
	nav_agent.path = nav_agent.map->GetPath(&nav_agent, { static_cast<float>(position.x), static_cast<float>(position.z) }, { nav_agent.target.x, nav_agent.target.z });

	// New path is empty, we are requesting an invalid path
	if (nav_agent.path.empty()) {
		return;
	}
}

void CGameObject::MoveToward(double x, double z) {
	if(nav_agent.target.x != x || nav_agent.target.z != z) {
		StartMove(x, z);
	}

	if (CompareFloat(nav_agent.target.x, position.x) && CompareFloat(nav_agent.target.z, position.z)) {
		DoAction(new GameObjectActionStop(this, nullptr));
		// Already at target
		return;
	}

	if (nav_agent.path.empty()) {
		// No path to follow, we need a new path!
		nav_agent.path = nav_agent.map->GetPath(&nav_agent, { static_cast<float>(position.x), static_cast<float>(position.z) }, { nav_agent.target.x, nav_agent.target.z });

		// New path is empty, we are requesting an invalid path
		if (nav_agent.path.empty()) {
			return;
		}
	}

	Vector2 vec2Move = nav_agent.map->Step(&nav_agent, {position.x, position.z}, modifiers.move_speed * (1000.0f / 60.0f) / 1000.0f);
	rotation.y = CalculateAngle({position.x, position.z}, {vec2Move.x, vec2Move.y});
	position.x = vec2Move.x;
	position.z = vec2Move.y;
	NetworkSyncComponent.SyncTransform = true;
}

void CGameObject::DoAction(GameObjectAction* newAction) {
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

void CGameObject::TakeDamage(float damage, CGameObject* source) {
	stats.health -= damage;

	if (stats.health <= 0) {
		// TODO die?
		stats.health = 0;
		Die();
	}

	NetworkSyncComponent.SyncStats = true;
}

void CGameObject::Die() {
	NetworkSyncComponent.SyncDespawn = true;
}
