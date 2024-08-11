#include <GameObject.h>
#include <Person.h>
#include <Common/PMG_Common.h>
#include <Game.h>
namespace PMG {
	void GameObjectActionCastSpell::Start() {
		person_->basic_attack_info.attack_started = false;
		person_->spell_cast_info.current_spell = -1;

		if (spell_index < 0 || spell_index >= person_->spells.size()) {
			// cannot cast spell that does not exist
			Logger::Err("Error: Attempt to cast spell that does not exist.");
			person_->DoAction(new GameObjectActionStop(person_, game_));
			return;
		}

		Spell* spell = person_->spells[spell_index];

		// no spell being cast yet
		if (person_->spell_cast_info.current_spell == -1) {
			if (spell->remaining_cooldown != -1) {
				return;
			}

			person_->spell_cast_info.current_spell = spell_index;
			person_->spell_cast_info.cast_time = 0;
			// TODO send packet to let clients know what's happening?

			spell->CastStart(person_, target_info);
		}
	}

	void GameObjectActionCastSpell::Do(float dt) {
		person_->spell_cast_info.cast_time += dt;
		
		Spell* spell = person_->spells[spell_index];

		if (person_->spell_cast_info.cast_time < (spell->cast_point / 1000.0f)) {
			// cast not completed yet
			return;
		}

		// spell cast successfully
		// wtf now?
		person_->spell_cast_info.current_spell = -1;
		person_->spell_cast_info.cast_time = 0;

		spell->Cast(game_, person_, target_info);

		spell->remaining_cooldown = spell->cooldown;
		person_->DoAction(new GameObjectActionStop(person_, game_));
	}

	void GameObjectActionCastSpell::Stop() {
		// TODO cancel cast correctly?
		person_->spell_cast_info.current_spell = -1;
		person_->spell_cast_info.cast_time = 0;
	}
}