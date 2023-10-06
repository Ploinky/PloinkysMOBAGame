#pragma once

#include <vector>
#include <string>
#include <functional>
#include "pmg_physics.h"
#include "GuiButton.h"
#include "GuiElement.h"

namespace PMG {
	class Renderer;
	class VBox : public GuiElement {
	public:
		bool m_hCenter = true;
		bool m_vCenter = true;
		int m_gap = 0;

		void LayoutChildren();
		void Render(Renderer* renderer);
	};

	class GuiSimplePane : public GuiElement {
	public:
		float m_color[3];

		void LayoutChildren();
		void Render(Renderer* renderer);
	};

	class GuiTextfield : public GuiElement {
	public:
		std::wstring m_text = L"";
		std::function<void(void)> e_onSubmit = {};

		void Render(Renderer* renderer);
		void CharTyped(char c);

	private:
		float m_bgColor[3] = { 1.0, 1.0, 1.0 };
		float m_color[3] = { 0.0, 0.0, 0.0 };
	};
}