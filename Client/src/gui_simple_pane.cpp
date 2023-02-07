#include "gui.h"
#include "renderer.h"

namespace PMG {
	void GuiSimplePane::LayoutChildren() {

	}

	void GuiSimplePane::Render(Renderer* renderer) {
		renderer->FillRect(m_pos.x, m_pos.y, m_size.x, m_size.y, m_color);
	}
}