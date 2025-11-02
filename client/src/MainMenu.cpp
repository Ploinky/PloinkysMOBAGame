#include "MainMenu.h"
#include "steam/isteamutils.h"
#include "steam/isteamuser.h"
#include "steam/isteamutils.h"
#include <common/PMG_Common.h>
#include "Game.h"

MainMenu::MainMenu(IClientStateHandler* handler, int width, int height) : IClientState(handler, width, height) {
	rootElement_.m_size = { static_cast<float>(windowWidth_), static_cast<float>(windowHeight_) };
	rootElement_.m_pos = { 0, 0 };
	HBitmap hButton = handler->GetAssetManager()->GetBitmapImage("UI/Buttons/MenuButton/MenuButton.bmp");

	// Server browser button
	buttonServerBrowser_.m_text = "Server Browser";
	buttonServerBrowser_.m_color[0] = 0.4f;
	buttonServerBrowser_.m_color[1] = 0.4f;
	buttonServerBrowser_.m_color[2] = 0.4f;
	buttonServerBrowser_.m_pos = { windowWidth_ - 400.0f, 50 };
	buttonServerBrowser_.m_size = { 300, 80 };
	buttonServerBrowser_.hImage = hButton;
	buttonServerBrowser_.e_onMousePressed = [this]() {
		handler_->OpenServerBrowser();
	};

	// Server browser button
	buttonSettings_.m_text = "Settings";
	buttonSettings_.m_color[0] = 0.4f;
	buttonSettings_.m_color[1] = 0.4f;
	buttonSettings_.m_color[2] = 0.4f;
	buttonSettings_.m_pos = { windowWidth_ - 400.0f, 180 };
	buttonSettings_.m_size = { 300, 80 };
	buttonSettings_.hImage = hButton;
	buttonSettings_.e_onMousePressed = [this]() {
		handler_->OpenSettingsMenu();
	};

	// Quit button
	buttonBack_.m_text = "Quit";
	buttonBack_.m_color[0] = 0.6f;
	buttonBack_.m_color[1] = 0.2f;
	buttonBack_.m_color[2] = 0.2f;
	buttonBack_.m_pos = { windowWidth_ - 400.0f, windowHeight_ - 150.0f };
	buttonBack_.m_size = { 300, 80 };
	buttonBack_.hImage = hButton;
	buttonBack_.e_onMousePressed = [this]() {
		NewState(nullptr);
	};

	rootElement_.m_children.push_back(&buttonServerBrowser_);
	rootElement_.m_children.push_back(&buttonSettings_);
	rootElement_.m_children.push_back(&buttonBack_);


	int iconId = SteamFriends()->GetMediumFriendAvatar(SteamUser()->GetSteamID());

	if (iconId != 0) {
		SteamUtils()->GetImageSize(iconId, &width_, &height_);

		userImage_.resize(4 * width_ * height_);

		if (!SteamUtils()->GetImageRGBA(iconId, userImage_.data(), 4 * width_ * height_)) {
			userImage_.resize(0);
		}
	}

	myName_ = std::string(SteamFriends()->GetPersonaName());

	SteamFriends()->SetRichPresence("steam_display", "#Status_AtMainMenu");
}

MainMenu::~MainMenu() {
}

void MainMenu::Render(CRenderer* renderer) {
	renderer->DrawMap();

	for (int x = 0; x < width_; x++) {
		for (int y = 0; y < height_; y++) {
			int first = (x + y * width_) * 4;
			float color[3] = { userImage_[first] / 255.0f, userImage_[first + 1] / 255.0f, userImage_[first + 2] / 255.0f };
			renderer->FillRect(50 + x, 50 + y, 1, 1, color);
		}
	}

	renderer->RenderText(50 + width_, 50, 200, 50, myName_);

	rootElement_.Render(renderer);
}

void MainMenu::Update(float dt) {
	// do nothing for now
	GuiElement* el = &buttonBack_;
	if (el->m_pos.x <= mouseX_ && el->m_pos.x + el->m_size.x >= mouseX_
		&& el->m_pos.y <= mouseY_ && el->m_pos.y + el->m_size.y >= mouseY_) {
		handler_->RequestCursor(CursorId::BUTTON_HOVER);
	}
}

void MainMenu::MouseButtonPressed(int button) {
	rootElement_.MousePressed(mouseX_, mouseY_);
}

void MainMenu::KeyPressed(uint32_t key) {
	if(key == 256) {
		handler_->NewState(nullptr);
	}
}