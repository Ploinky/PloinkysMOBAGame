#include "GuiServerElement.h"
#include "Renderer.h"
#include <string>

Server_t GuiServerElement::GetServer() {
	return server_;
}

void GuiServerElement::Render(CRenderer* renderer) {
	float red[3]{ 1, 0, 0 };
	float white[3]{ 1, 1, 1 };

	renderer->FillRect(m_pos.x, m_pos.y, 400, 30, white);
	renderer->RenderText(m_pos.x, m_pos.y, 200, 30, red, std::string(server_.name));
	renderer->RenderText(m_pos.x + 200, m_pos.y, 200, 30, red, std::string(server_.addr.GetConnectionAddressString()));
}
