#pragma once

#include "GuiElement.h"
#include <functional>
#include <string>

namespace PMG {
	class GuiButton : public GuiElement {
	public:
		float m_backgroundColor[3] = { 0.0, 0.0, 0.0 };
		float m_color[3] = { 1.0, 1.0, 1.0 };
		std::wstring m_text = L"";
		std::function<void(void)> e_onButtonPressed = {};

		void Render(Renderer* renderer);
		void MousePressed(int x, int y);
	};
}