#include "GuiCheckbox.h"

void GuiCheckbox::Render(CRenderer* renderer) {
	float color[3]{ 1, 1, 1 };

	if (isSelected_) {
		renderer->FillRect(m_pos.x, m_pos.y, m_size.x, m_size.y, color);
	}
	else {
		renderer->DrawRect(m_pos.x, m_pos.y, m_size.x, m_size.y, color);
	}
}

void GuiCheckbox::MousePressed(int x, int y) {
	isSelected_ = !isSelected_;
}

bool GuiCheckbox::IsSelected() {
	return isSelected_;
}
