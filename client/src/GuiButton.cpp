#include "GuiButton.h"
#include "Renderer.h"

void GuiButton::Render(CRenderer* renderer) {
	renderer->DrawImage(m_pos.x, m_pos.y, m_size.x, m_size.y, "MenuButton");
	renderer->RenderText(m_pos.x, m_pos.y, m_size.x, m_size.y, m_text);
}
