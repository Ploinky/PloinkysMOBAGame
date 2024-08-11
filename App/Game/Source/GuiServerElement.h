#pragma once

#include "GuiElement.h"
#include "Server_t.h"

namespace PMG {
	class GuiServerElement : public GuiElement {
	public:
		GuiServerElement(Server_t server) : server_(server) {};
		Server_t GetServer();

		void Render(CRenderer* renderer);

	private:
		Server_t server_;
	};
}