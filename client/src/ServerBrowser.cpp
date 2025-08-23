#include "ServerBrowser.h"
#include "Renderer.h"
#include <Common/PMG_Common.h>

ServerBrowser::ServerBrowser(IClientStateHandler* handler, int width, int height) : IClientState(handler, width, height) {
	HBitmap hBitmap = handler->GetAssetManager()->LoadBitmapImage("UI/Buttons/MenuButton/MenuButton.bmp");

	rootElement_.m_size = { static_cast<float>(windowWidth_), static_cast<float>(windowHeight_) };
	rootElement_.m_pos = { 0, 0 };

	buttonRefresh_.hImage = hBitmap;
	buttonRefresh_.m_text = "Refresh";
	buttonRefresh_.m_color[0] = 0.4f;
	buttonRefresh_.m_color[1] = 0.4f;
	buttonRefresh_.m_color[2] = 0.4f;
	buttonRefresh_.m_pos = { 50, 50 };
	buttonRefresh_.m_size = { 300, 80 };
	buttonRefresh_.e_onMousePressed = [this]() {
		buttonRefresh_.m_color[0] = 0.2f;
		buttonRefresh_.m_color[1] = 0.2f;
		buttonRefresh_.m_color[2] = 0.2f;
		StartRefresh();
	};

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
		
	checkboxLan_.m_pos = { 370, 50 };
	checkboxLan_.m_size = { 20, 20 };

	rootElement_.m_children.push_back(&buttonRefresh_);
	rootElement_.m_children.push_back(&buttonBack_);
	rootElement_.m_children.push_back(&checkboxLan_);
}

ServerBrowser::~ServerBrowser() {
	CancelRefreshRequest();
}


void ServerBrowser::Update(float dt) {

}

void ServerBrowser::Render(CRenderer* renderer) {
	renderer->RenderText(checkboxLan_.m_pos.x + 30, checkboxLan_.m_pos.y, 100, 20, "Lan servers");

	rootElement_.Render(renderer);
}

void ServerBrowser::MouseButtonPressed(int button) {
	rootElement_.MousePressed(mouseX_, mouseY_);
}

void ServerBrowser::StartRefresh() {
	// servers_.clear();
}


void ServerBrowser::CancelRefreshRequest() {
	buttonRefresh_.m_color[0] = 0.4f;
	buttonRefresh_.m_color[1] = 0.4f;
	buttonRefresh_.m_color[2] = 0.4f;
	buttonRefresh_.m_text = "Refresh";
}
