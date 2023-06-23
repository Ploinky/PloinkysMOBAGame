#pragma once

#include "gui.h"
#include "renderer.h"
#include <string>

namespace PMG {
	template<typename T>
	class GuiSelector : public GuiElement {
	public:
		GuiSelector(std::vector<T> values, T default_value) {
			this->values_ = values;
			current_value_ = default_value;
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
		void MousePressed(int x, int y) {
			for (auto it = values_.begin(); it != values_.end(); ++it) {
				if (*it == current_value_) {
					auto next = it + 1;
					if (next == values_.end()) {
						next = values_.begin();
					}

					current_value_ = *next;
					OnChange(*it, current_value_);
					break;
				}
			}
		};
		float m_color[3] = { 0.0, 0.0, 0.0 };

		std::function<std::wstring(T val)> OptionToString;

	private:
		float m_bgColor[3] = { 1.0, 1.0, 1.0 };
		std::vector<T> values_;
		T current_value_;
	};
}