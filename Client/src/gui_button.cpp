#include "gui.h"
#include "renderer.h"

namespace PMG {
	void GuiButton::Render(Renderer* renderer) {
		renderer->FillRect(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), static_cast<int>(m_size.x), static_cast<int>(m_size.y), m_color);
		renderer->RenderText(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), static_cast<int>(m_size.x), static_cast<int>(m_size.y), m_text);
	}

	void GuiButton::MousePressed(int x, int y) {
		if (e_onButtonPressed != nullptr) {
			e_onButtonPressed();
		}
	}
}