#pragma once

#include <vector>
#include <string>
#include <functional>
#include "pmg_physics.h"

namespace PMG {
	class Renderer;

	class GuiElement {
	public:
		// Position of top left corner
		Physics::Vector2 m_pos = Physics::Vector2(0, 0);
		Physics::Vector2 m_size = Physics::Vector2(0, 0);
		Physics::Vector2 m_prefSize = Physics::Vector2(0, 0);

		std::vector<GuiElement*> m_children;

		virtual void LayoutChildren() {};
		virtual void Render(Renderer* renderer) {};
		virtual void MousePressed(int x, int y) {
			for (GuiElement* el : m_children) {
				if (el->m_pos.x <= x && el->m_pos.x + el->m_size.x >= x
					&& el->m_pos.y <= y && el->m_pos.y + el->m_size.y >= y) {
					el->MousePressed(x, y);
				}
			}
		};
		virtual void MouseMoved(int x, int y) {
			for (GuiElement* el : m_children) {
				if (el->m_pos.x <= x && el->m_pos.x + el->m_size.x >= x
					&& el->m_pos.y <= y && el->m_pos.y + el->m_size.y >= y) {
					el->MouseMoved(x, y);
				}
			}
		};
		virtual void CharTyped(char c) {
			for (GuiElement* el : m_children) {
				el->CharTyped(c);
			}
		};
		virtual void KeyPressed(char c) {};
		virtual void KeyReleased(char c) {};
	};

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

	class GuiButton : public GuiElement {
	public:
		float m_backgroundColor[3] = { 0.0, 0.0, 0.0 };
		float m_color[3] = { 1.0, 1.0, 1.0 };
		std::wstring m_text = L"";
		std::function<void(void)> e_onButtonPressed = {};

		void Render(Renderer* renderer);
		void MousePressed(int x, int y);
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