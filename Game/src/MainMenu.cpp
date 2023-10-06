#include "MainMenu.h"
#include "steam/isteamutils.h"
#include "steam/isteamuser.h"
#include "steam/isteamutils.h"
#include "util.h"
#include "Game.h"

namespace PMG {
	MainMenu::MainMenu(IClientStateHandler* handler, int width, int height) : IClientState(handler, width, height) {
		// Server browser button
		buttonServerBrowser_.m_text = L"Server Browser";
		buttonServerBrowser_.m_color[0] = 0.4f;
		buttonServerBrowser_.m_color[1] = 0.4f;
		buttonServerBrowser_.m_color[2] = 0.4f;
		buttonServerBrowser_.m_pos = { windowWidth_ - 400.0f, 50 };
		buttonServerBrowser_.m_size = { 300, 80 };
		buttonServerBrowser_.e_onButtonPressed = [this]() {
			handler_->OpenServerBrowser();
		};

		// Server browser button
		buttonSettings_.m_text = L"Settings";
		buttonSettings_.m_color[0] = 0.4f;
		buttonSettings_.m_color[1] = 0.4f;
		buttonSettings_.m_color[2] = 0.4f;
		buttonSettings_.m_pos = { windowWidth_ - 400.0f, 180 };
		buttonSettings_.m_size = { 300, 80 };
		buttonSettings_.e_onButtonPressed = [this]() {
			handler_->OpenSettingsMenu();
		};

		// Quit button
		buttonBack_.m_text = L"Quit";
		buttonBack_.m_color[0] = 0.6f;
		buttonBack_.m_color[1] = 0.2f;
		buttonBack_.m_color[2] = 0.2f;
		buttonBack_.m_pos = { windowWidth_ - 400.0f, windowHeight_ - 150.0f };
		buttonBack_.m_size = { 300, 80 };
		buttonBack_.e_onButtonPressed = [this]() {
			NewState(nullptr);
		};

		int iconId = SteamFriends()->GetMediumFriendAvatar(SteamUser()->GetSteamID());

		if (iconId != 0) {
			SteamUtils()->GetImageSize(iconId, &width_, &height_);

			userImage_.resize(4 * width_ * height_);

			if (!SteamUtils()->GetImageRGBA(iconId, userImage_.data(), 4 * width_ * height_)) {
				userImage_.resize(0);
			}
		}

		myName_ = std::string(SteamFriends()->GetPersonaName());
	}

	MainMenu::~MainMenu() {
	}

	void MainMenu::Render(Renderer* renderer) {
		for (int x = 0; x < width_; x++) {
			for (int y = 0; y < height_; y++) {
				int first = (x + y * width_) * 4;
				float color[3] = { userImage_[first] / 255.0f, userImage_[first + 1] / 255.0f, userImage_[first + 2] / 255.0f };
				renderer->FillRect(50 + x, 50 + y, 1, 1, color);
			}
		}

		renderer->RenderText(50 + width_, 50, 200, 50, Util::string_to_wstring(myName_));

		buttonServerBrowser_.Render(renderer);
		buttonSettings_.Render(renderer);
		buttonBack_.Render(renderer);
	}

	void MainMenu::Update(float dt) {
		// do nothing for now
	}

	void MainMenu::MouseButtonPressed(int button) {
		// TODO i do not want to do this every time for every button yikes
		if (mouseX_ >= buttonBack_.m_pos.x && mouseX_ <= buttonBack_.m_pos.x + buttonBack_.m_size.x
			&& mouseY_ >= buttonBack_.m_pos.y && mouseY_ <= buttonBack_.m_pos.y + buttonBack_.m_size.y) {
			buttonBack_.MousePressed(mouseX_, mouseY_);
		}

		// TODO i do not want to do this every time for every button yikes
		if (mouseX_ >= buttonServerBrowser_.m_pos.x && mouseX_ <= buttonServerBrowser_.m_pos.x + buttonServerBrowser_.m_size.x
			&& mouseY_ >= buttonServerBrowser_.m_pos.y && mouseY_ <= buttonServerBrowser_.m_pos.y + buttonServerBrowser_.m_size.y) {
			buttonServerBrowser_.MousePressed(mouseX_, mouseY_);
		}

		// TODO i do not want to do this every time for every button yikes
		if (mouseX_ >= buttonSettings_.m_pos.x && mouseX_ <= buttonSettings_.m_pos.x + buttonSettings_.m_size.x
			&& mouseY_ >= buttonSettings_.m_pos.y && mouseY_ <= buttonSettings_.m_pos.y + buttonSettings_.m_size.y) {
			buttonSettings_.MousePressed(mouseX_, mouseY_);
		}
	}
}