#pragma once

#include "common/pmg_physics.h"
#include <vector>
#include <functional>

class CRenderer;

class GuiElement {
public:
	// Position of top left corner
	Vector2 m_pos = Vector2(0, 0);
	Vector2 m_size = Vector2(0, 0);
	Vector2 m_prefSize = Vector2(0, 0);

	std::vector<GuiElement*> m_children;


	std::function<void(void)> e_onMousePressed = nullptr;

	virtual void LayoutChildren() {};
	virtual void Render(CRenderer* renderer) {
		for (GuiElement* el : m_children) {
			el->Render(renderer);
		}
	};

	virtual void MousePressed(int x, int y) {
		if (e_onMousePressed != nullptr) {
			e_onMousePressed();
		}

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
