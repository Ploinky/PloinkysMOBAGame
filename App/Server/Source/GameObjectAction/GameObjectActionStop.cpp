#include <GameObject.h>
#include <Person.h>

namespace PMG {
	void GameObjectActionStop::Start() {
		self_->basic_attack_info.attack_started = false;
		self_->spell_cast_info.current_spell = -1;

		self_->nav_agent.target = { self_->position.x, self_->position.z };
		self_->nav_agent.path.clear();
	}

	void GameObjectActionStop::Do(float dt) {
	}

	void GameObjectActionStop::Stop() {
	}

}