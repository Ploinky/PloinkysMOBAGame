#include "SettingsMenu.h"
#include "util.h"

namespace PMG {
	SettingsMenu::SettingsMenu(IClientStateHandler* handler, int width, int height, Settings* settings)
		: IClientState(handler, width, height), settings_(settings) {
		mouseX_ = 0;
		mouseY_ = 0;
		// Quit button
		buttonBack_.m_text = "Back";
		buttonBack_.m_color[0] = 0.4f;
		buttonBack_.m_color[1] = 0.4f;
		buttonBack_.m_color[2] = 0.4f;
		buttonBack_.m_pos = { windowWidth_ - 400.0f, windowHeight_ - 150.0f };
		buttonBack_.m_size = { 300, 80 };
		buttonBack_.e_onButtonPressed = [this]() {
			handler_->OpenMainMenu();
		};

		guiSelector_.m_pos = { 140, 60 };
		guiSelector_.m_size = { 300, 60 };
		guiSelector_.OptionToString = [](WindowMode val) {
			switch (val) {
			case WindowMode::WINDOWED:
				return std::string("Windowed");
			case WindowMode::BORDERLESS:
				return std::string("Borderless");
			case WindowMode::FULLSCREEN:
				return std::string("Fullscreen");
			default:
				return std::string();
			}
		};
		guiSelector_.OnChange = [this](WindowMode oldValue, WindowMode newValue) {
			settings_->SetInt(PMGSettings::WINDOW_MODE, (int)newValue);
		};
		guiSelector_.m_color[0] = 1;
		guiSelector_.m_color[1] = 1;
		guiSelector_.m_color[2] = 1;
		
	}

	SettingsMenu::~SettingsMenu() {
	}

	void SettingsMenu::Render(Renderer* renderer) {
		renderer->RenderText(30, 30, 100, 30, "Resolution:");
		renderer->RenderText(130, 30, 100, 30, settings_->GetString(PMGSettings::VIDEO_MODE));


		if (videoModeExtended_) {
			int i = 1;
			for (std::string mode : settings_->GetAllVideoModes()) {
				float color[3] = { 0.4, 0.4, 0.4 };
				renderer->FillRect(130, 30 + i * 30, 100, 30, color);
				renderer->RenderText(130, 30 + i * 30, 100, 30, mode.c_str());

				i++;
			}
		}

		renderer->RenderText(30, 60, 100, 30, "Window mode:");
		guiSelector_.Render(renderer);

		buttonBack_.Render(renderer);
	}

	void SettingsMenu::Update(float dt) {
	}

	void SettingsMenu::MouseButtonPressed(int button) {
		if (button != 0) {
			return;
		}

		// TODO i do not want to do this every time for every button yikes
		if (mouseX_ >= 130 && mouseX_ <= 230
			&& mouseY_ >= 30 && mouseY_ <= 60) {
			videoModeExtended_ = !videoModeExtended_;
		}

		if (videoModeExtended_) {
			int i = 1;
			for (std::string mode : settings_->GetAllVideoModes()) {
				float color[3] = { 0.4, 0.4, 0.4 };
				// TODO i do not want to do this every time for every button yikes
				if (mouseX_ >= 130 && mouseX_ <= 230
					&& mouseY_ >= 30 + i * 30 && mouseY_ <= 30 + i * 30 + 30) {
					settings_->SetString(PMGSettings::VIDEO_MODE, mode.c_str());
					videoModeExtended_ = !videoModeExtended_;
					return;
				}
				i++;
			}
		}

		// TODO i do not want to do this every time for every button yikes
		if (mouseX_ >= buttonBack_.m_pos.x && mouseX_ <= buttonBack_.m_pos.x + buttonBack_.m_size.x
			&& mouseY_ >= buttonBack_.m_pos.y && mouseY_ <= buttonBack_.m_pos.y + buttonBack_.m_size.y) {
			buttonBack_.MousePressed(mouseX_, mouseY_);
		}

		// TODO i do not want to do this every time for every button yikes
		if (mouseX_ >= guiSelector_.m_pos.x && mouseX_ <= guiSelector_.m_pos.x + guiSelector_.m_size.x
			&& mouseY_ >= guiSelector_.m_pos.y && mouseY_ <= guiSelector_.m_pos.y + guiSelector_.m_size.y) {
			guiSelector_.MousePressed(mouseX_, mouseY_);
		}
	}

	void SettingsMenu::OnWindowResized() {
		buttonBack_.m_pos = { windowWidth_ - 400.0f, windowHeight_ - 150.0f };
	}
}