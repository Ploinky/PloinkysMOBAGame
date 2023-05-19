#include "gui.h"
#include "renderer.h"

namespace PMG {
	void GuiTextfield::Render(Renderer* renderer) {
		renderer->FillRect(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), static_cast<int>(m_size.x), static_cast<int>(m_size.y), m_bgColor);
		renderer->RenderText(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), static_cast<int>(m_size.x), static_cast<int>(m_size.y), m_color, m_text);
	}

	void GuiTextfield::CharTyped(char c) {
		if (!m_text.empty() && c == VK_BACK) {
			m_text.pop_back();
			return;
		}

		m_text.push_back(c);
	}
}