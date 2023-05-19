#pragma once

#include "gui.h"
#include "renderer.h"
#include <string>

namespace PMG {
	template<typename T>
	class GuiSelector : public GuiElement {
	public:
		GuiSelector(std::vector<T> values) {
			this->values_ = values;
			current_value_ = values.at(0);
		}
		std::function<void(T oldValue, T newValue)> OnChange = {};

		void Render(Renderer* renderer) {
			renderer->DrawRect(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), static_cast<int>(m_size.x), static_cast<int>(m_size.y), m_bgColor);
			renderer->RenderText(
				static_cast<int>(m_pos.x),
				static_cast<int>(m_pos.y),
				static_cast<int>(m_size.x),
				static_cast<int>(m_size.y),
				m_color,
				OptionToString(current_value_));
		}
		void MousePressed(int x, int y) {};
		float m_color[3] = { 0.0, 0.0, 0.0 };

		std::function<std::wstring(T val)> OptionToString;

	private:
		float m_bgColor[3] = { 1.0, 1.0, 1.0 };
		std::vector<T> values_;
		T current_value_;
	};
}