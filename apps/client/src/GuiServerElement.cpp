#include "GuiServerElement.h"
#include "Renderer.h"
#include <string>

GuiServerElement::GuiServerElement(Server_t server) : server_(server) {
	m_prefSize = {-1,30};
}

Server_t GuiServerElement::GetServer() {
	return server_;
}

void GuiServerElement::Render(CRenderer* renderer) {
	float red[3]{ 1, 0, 0 };
	float white[3]{ 1, 1, 1 };

	renderer->FillRect(m_pos.x, m_pos.y, m_size.x, m_size.y, white);
	renderer->RenderText(m_pos.x, m_pos.y, (m_size.x / 2), m_size.y, red, std::string(server_.name));
	renderer->RenderText(m_pos.x + (m_size.x / 2), m_pos.y, m_size.x / 2, m_size.y, red, server_.addr);
}
