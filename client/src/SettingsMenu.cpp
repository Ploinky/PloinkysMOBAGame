#include "SettingsMenu.h"
#include <Common/PMG_Common.h>

SettingsMenu::SettingsMenu(IClientStateHandler* handler, int width, int height, Settings* settings)
	: IClientState(handler, width, height), settings_(settings) {

	HBitmap hBitmap = handler->GetAssetManager()->LoadBitmapImage("UI/Buttons/MenuButton/MenuButton.bmp");

	rootElement_.m_size = { static_cast<float>(windowWidth_), static_cast<float>(windowHeight_) };
	rootElement_.m_pos = { 0, 0 };

	mouseX_ = 0;
	mouseY_ = 0;
	// Quit button
	buttonBack_.hImage = hBitmap;
	buttonBack_.m_text = "Back";
	buttonBack_.m_color[0] = 0.4f;
	buttonBack_.m_color[1] = 0.4f;
	buttonBack_.m_color[2] = 0.4f;
	buttonBack_.m_pos = { windowWidth_ - 400.0f, windowHeight_ - 150.0f };
	buttonBack_.m_size = { 300, 80 };
	buttonBack_.e_onMousePressed = [this]() {
		handler_->OpenMainMenu();
	};

	guiSelector_.m_pos = { 140, 60 };
	guiSelector_.m_size = { 200, 30 };
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
	guiSelector_.SelectOption((WindowMode) settings_->GetInt(PMGSettings::WINDOW_MODE));

	rootElement_.m_children.push_back(&buttonBack_);
	rootElement_.m_children.push_back(&guiSelector_);
}

SettingsMenu::~SettingsMenu() {
}

void SettingsMenu::Render(CRenderer* renderer) {
	renderer->RenderText(30, 60, 100, 30, "Windowmode:");

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

	rootElement_.Render(renderer);
}

void SettingsMenu::Update(float dt) {
}

void SettingsMenu::MouseButtonPressed(int button) {
	if (button != 0) {
		return;
	}

	// TODO i do not want to do this every time for every button yikes
	if (!videoModeExtended_ && mouseX_ >= 130 && mouseX_ <= 230
		&& mouseY_ >= 30 && mouseY_ <= 60) {
		videoModeExtended_ = true;
		return;
	}


	if (videoModeExtended_) {
		int i = 1;
		for (std::string mode : settings_->GetAllVideoModes()) {
			float color[3] = { 0.4, 0.4, 0.4 };
			// TODO i do not want to do this every time for every button yikes
			if (mouseX_ >= 130 && mouseX_ <= 230
				&& mouseY_ >= 30 + i * 30 && mouseY_ <= 30 + i * 30 + 30) {
				settings_->SetString(PMGSettings::VIDEO_MODE, mode.c_str());
				videoModeExtended_ = false;
				return;
			}
			i++;
		}
	}

	videoModeExtended_ = false;

	rootElement_.MousePressed(mouseX_, mouseY_);
}

void SettingsMenu::OnWindowResized() {
	buttonBack_.m_pos = { windowWidth_ - 400.0f, windowHeight_ - 150.0f };
}
