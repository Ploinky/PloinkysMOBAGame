#pragma once

#include "GuiElement.h"
#include <string>
#include "client-asset-manager.h"

class GuiButton : public GuiElement {
public:
	float m_backgroundColor[3] = { 0.0, 0.0, 0.0 };
	float m_color[3] = { 1.0, 1.0, 1.0 };
	std::string m_text = "";

	void Render(CRenderer* renderer);

	HBitmap hImage;
};
