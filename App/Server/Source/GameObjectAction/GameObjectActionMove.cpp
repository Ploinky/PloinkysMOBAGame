#include <GameObject.h>
#include <Person.h>

namespace PMG {
	void GameObjectActionMove::Start() {
        person_->basic_attack_info.attack_started = false;
        person_->spell_cast_info.current_spell = -1;
        
        StartMove(target_point.x, target_point.y);
	}
	
	void GameObjectActionMove::Do(float dt) {
        MoveToward(target_point.x, target_point.y);

	}
	
	void GameObjectActionMove::Stop() {

	}


	void GameObjectActionMove::StartMove(double x, double z) {
		person_->nav_agent.path.clear();

		person_->nav_agent.target.x = x;
		person_->nav_agent.target.z = z;

		// No path to follow, we need a new path!
		person_->nav_agent.path = person_->nav_agent.grid->GetPath({ static_cast<float>(person_->position.x), static_cast<float>(person_->position.z) }, { person_->nav_agent.target.x, person_->nav_agent.target.z });

		if (person_->nav_agent.path.empty()) {
			return;
		}
	}
	void GameObjectActionMove::MoveToward(double x, double z) {
		person_->nav_agent.target.x = x;
		person_->nav_agent.target.z = z;

		if (Physics::CompareFloat(person_->nav_agent.target.x, person_->position.x) && Physics::CompareFloat(person_->nav_agent.target.z, person_->position.z)) {
			person_->DoAction(new GameObjectActionStop(person_, game_));
			// Already at target
			return;
		}

		if (person_->nav_agent.path.empty()) {
			// No path to follow, we need a new path!
			person_->nav_agent.path = person_->nav_agent.grid->GetPath({ static_cast<float>(person_->position.x), static_cast<float>(person_->position.z) }, { person_->nav_agent.target.x, person_->nav_agent.target.z });

			// New path is empty, we are requesting an invalid path
			if (person_->nav_agent.path.empty()) {
				person_->DoAction(new GameObjectActionStop(person_, game_));
				return;
			}
		}

		Physics::Vector2 intermediateTarget = person_->nav_agent.path.front();

		if (abs(person_->position.x - intermediateTarget.x) < 0.001 && abs(person_->position.z - intermediateTarget.y) < 0.001) {
			// Next frame we follow next?!
			person_->nav_agent.path.erase(person_->nav_agent.path.begin());
			person_->nav_agent.path = person_->nav_agent.grid->GetPath({ static_cast<float>(person_->position.x), static_cast<float>(person_->position.z) }, { person_->nav_agent.target.x, person_->nav_agent.target.z });


			if (abs(person_->position.x - intermediateTarget.x) < 0.001 && abs(person_->position.z - intermediateTarget.y) < 0.001) {
				person_->nav_agent.path.erase(person_->nav_agent.path.begin());
			}

			if (!person_->nav_agent.path.empty()) {
				intermediateTarget = person_->nav_agent.path.front();
			}
		}

		float tx = intermediateTarget.x;
		float ty = intermediateTarget.y;

		if (Physics::CompareFloat(person_->position.x, tx) && Physics::CompareFloat(person_->position.z, ty)) {
			return;
		}

		float dx = tx - person_->position.x;
		float dy = ty - person_->position.z;
		float length = sqrt(dx * dx + dy * dy);


		dx /= length;
		dy /= length;

		float newX = person_->position.x + person_->modifiers.move_speed * dx * (1000.0f / 60.0f) / 1000.0f;
		float newY = person_->position.z + person_->modifiers.move_speed * dy * (1000.0f / 60.0f) / 1000.0f;

		NavigationCell* currentCell = person_->nav_agent.grid->GetCellAt(person_->position.x, person_->position.z);
		NavigationCell* nextCell = person_->nav_agent.grid->GetCellAt(newX, newY);

		if(currentCell != nextCell) {
			if(!nextCell->IsOpen || !nextCell->IsWalkable) {
				// running into occupied cell, renav next frame!
				person_->nav_agent.path = {};
				// person_->nav_agent.grid->GetPath({ static_cast<float>(person_->position.x), static_cast<float>(person_->position.z) }, { person_->nav_agent.target.x, person_->nav_agent.target.z });
				// quit because we do not actually move now
				return;
			}
		}

		person_->position.x = (person_->position.x < tx && newX >= tx) || (person_->position.x > tx && newX <= tx) ? tx : newX;
		person_->position.z = (person_->position.z < ty && newY >= ty) || (person_->position.z > ty && newY <= ty) ? ty : newY;

		if (person_->position.x != tx || person_->position.z != ty) {
			person_->rotation.y = Physics::CalculateAngle({person_->position.x, person_->position.z}, {tx, ty});
		}
	}
}