#include "Gui.h"
#include "Renderer.h"

void GuiTextfield::Render(CRenderer* renderer) {
	renderer->FillRect(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), static_cast<int>(m_size.x), static_cast<int>(m_size.y), m_bgColor);
	renderer->RenderText(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), static_cast<int>(m_size.x), static_cast<int>(m_size.y), m_color, m_text);
}

void GuiTextfield::CharTyped(char c) {
	throw std::runtime_error("this is broken right now");
	/*
	if (!m_text.empty() && c == ' ') {
		m_text.pop_back();
		return;
	}
	
	m_text.push_back(c);
	*/
}
