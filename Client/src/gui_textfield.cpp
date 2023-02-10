#include "gui.h"
#include "renderer.h"

namespace PMG {
	void GuiTextfield::Render(Renderer* renderer) {
		renderer->FillRect(m_pos.x, m_pos.y, m_size.x, m_size.y, m_bgColor);
		renderer->RenderText(m_pos.x, m_pos.y, m_size.x, m_size.y, m_color, m_text);
	}

	void GuiTextfield::CharTyped(char c) {
		if (!m_text.empty() && c == VK_BACK) {
			m_text.pop_back();
			return;
		}

		m_text.push_back(c);
	}
}