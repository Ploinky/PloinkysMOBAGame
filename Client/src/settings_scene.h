#pragma once

#include "scene.h"
#include "settings.h"

namespace PMG {
	class SettingsScene : public Scene {
	public:
		SettingsScene(ClientStateHandler* stateHandler, Settings* settings);

		void Update(float dt);
		void Render(Renderer* renderer);
		void CharTyped(uint32_t ch);
		void KeyPressed(uint32_t key);
		void KeyReleased(uint32_t key);
		void MouseMoved(int screenX, int screenY);
		void MouseButtonPressed(int button);
		void MouseButtonReleased(int button);

		GuiElement* rootGuiElement;
		int m_mousePos[2]{ 0 };

	private:
		Settings* settings_;
	};
}