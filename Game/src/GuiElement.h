#pragma once

#include "common/pmg_physics.h"
#include <vector>

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
		virtual void Render(Renderer* renderer) {
			for (GuiElement* el : m_children) {
				el->Render(renderer);
			}
		};

		virtual void MousePressed(int x, int y) {
			for (GuiElement* el : m_children) {
				if (el->m_pos.x <= x && el->m_pos.x + el->m_size.x >= x
					&& el->m_pos.y <= y && el->m_pos.y + el->m_size.y >= y) {
					el->MousePressed(x, y);
					break;
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
}