#include "gui.h"
#include "renderer.h"

namespace PMG {
	void GuiButton::Render(Renderer* renderer) {
		renderer->FillRect(m_pos.x, m_pos.y, m_size.x, m_size.y, m_color);
		renderer->RenderText(m_pos.x, m_pos.y, m_size.x, m_size.y, m_text);
	}

	void GuiButton::MousePressed(int x, int y) {
		if (e_onButtonPressed != nullptr) {
			e_onButtonPressed();
		}
	}
}