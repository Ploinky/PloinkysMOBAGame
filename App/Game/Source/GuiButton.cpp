#include "GuiButton.h"
#include "Renderer.h"

namespace PMG {
	void GuiButton::Render(CRenderer* renderer) {
		renderer->DrawImage(m_pos.x, m_pos.y, m_size.x, m_size.y, BitmapId::BUTTON_MENU);
		renderer->RenderText(m_pos.x, m_pos.y, m_size.x, m_size.y, m_text);
	}
}