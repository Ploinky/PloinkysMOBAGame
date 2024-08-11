#pragma once

#include "Gui.h"
#include "Renderer.h"
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

		void Render(CRenderer* renderer) {
			renderer->DrawRect(static_cast<int>(m_pos.x), static_cast<int>(m_pos.y), static_cast<int>(m_size.x), static_cast<int>(m_size.y), m_bgColor);
			renderer->RenderText(
				static_cast<int>(m_pos.x),
				static_cast<int>(m_pos.y),
				static_cast<int>(m_size.x),
				static_cast<int>(m_size.y),
				m_color,
				OptionToString(current_value_));

			Physics::Vector2 points[3] = {
				{ m_pos.x + 10, m_pos.y + m_size.y / 2 },
				{ m_pos.x + m_size.y - 10, m_pos.y + 10 },
				{ m_pos.x + m_size.y - 10, m_pos.y + m_size.y - 10 },
			};
			renderer->FillShape(points, 3, m_color);

			Physics::Vector2 rightPoints[3] = {
				{ m_pos.x + m_size.x - 10, m_pos.y + m_size.y / 2 },
				{ m_pos.x + m_size.x - m_size.y + 10, m_pos.y + 10 },
				{ m_pos.x + m_size.x - m_size.y + 10, m_pos.y + m_size.y - 10 },
			};
			renderer->FillShape(rightPoints, 3, m_color);
		}
		void MousePressed(int x, int y) {
			bool back = false;

			if (x < (m_pos.x + (m_size.x / 2))) {
				back = true;
			}

			if (back) {
				for (auto it = values_.rbegin(); it != values_.rend(); ++it) {
					if (*it == current_value_) {
						auto last = it + 1;
						if (last == values_.rend()) {
							last = values_.rbegin();
						}

						current_value_ = *last;
						OnChange(*it, current_value_);
						break;
					}
				}
			}
			else {
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
			}

		};
		float m_color[3] = { 0.0, 0.0, 0.0 };

		std::function<std::string(T val)> OptionToString;

		std::vector<T> GetOptions() {
			return values_;
		}

		void AddOption(T option) {
			values_.push_back(option);
		}

		void RemoveOption(T option) {
			values_.erase(option);
		}

		void SelectOption(T option) {
			current_value_ = option;
		}
	private:
		float m_bgColor[3] = { 1.0, 1.0, 1.0 };
		std::vector<T> values_;
		T current_value_;
	};
}