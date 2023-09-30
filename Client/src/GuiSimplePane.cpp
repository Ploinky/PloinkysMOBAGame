#include "Gui.h"
#include "Renderer.h"

namespace PMG {
	void GuiSimplePane::LayoutChildren() {

	}

	void GuiSimplePane::Render(Renderer* renderer) {
		renderer->FillRect(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), static_cast<int>(m_size.x), static_cast<int>(m_size.y), m_color);
	}
}