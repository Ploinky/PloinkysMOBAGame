#include "MainMenu.h"
#include <common/PMG_Common.h>
#include "Game.h"

MainMenu::MainMenu(IClientStateHandler* handler, int width, int height) : IClientState(handler, width, height) {
	rootElement_.m_size = { static_cast<float>(windowWidth_), static_cast<float>(windowHeight_) };
	rootElement_.m_pos = { 0, 0 };
	HBitmap hButton = handler->GetAssetManager()->GetBitmapImage("menu_button");

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

	// TODO
	myName_ = std::string("Ploinky");
}

MainMenu::~MainMenu() {
}

void MainMenu::Action(EInputAction eAction) {
	switch(eAction) {
		case EInputAction::MENU_SELECT:
			rootElement_.MousePressed(mouseX_, mouseY_);
			break;
		case EInputAction::MENU_CLOSE:
			NewState(nullptr);
			break;
		default:
			return;
	}
}

void MainMenu::Render(CRenderer* renderer) {
	renderer->DrawMap();

	renderer->RenderText(50, 50, 200, 50, myName_);

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