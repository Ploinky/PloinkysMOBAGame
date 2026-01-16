#include "ServerBrowser.h"
#include "Renderer.h"
#include <common/PMG_Common.h>

ServerBrowser::ServerBrowser(IClientStateHandler* handler, int width, int height) : IClientState(handler, width, height) {
	m_netManager = ClientNetworkManager();
	m_netManager.Initialize(handler->GetNetworkEngine(), nullptr);
	m_netManager.RegisterRequestObserver(this);

	rootElement_.m_size = { static_cast<float>(windowWidth_), static_cast<float>(windowHeight_) };
	rootElement_.m_pos = { 0, 0 };
	HBitmap hButton = handler->GetAssetManager()->GetBitmapImage("menu_button");

	buttonRefresh_.m_text = "Refresh";
	buttonRefresh_.m_color[0] = 0.4f;
	buttonRefresh_.m_color[1] = 0.4f;
	buttonRefresh_.m_color[2] = 0.4f;
	buttonRefresh_.m_pos = { 50, 50 };
	buttonRefresh_.m_size = { 300, 80 };
	buttonRefresh_.hImage = hButton;
	buttonRefresh_.e_onMousePressed = [this]() {
		if (refreshRequest_ != INVALID_HANDLE) {
			CancelRefreshRequest();
			return;
		}

		buttonRefresh_.m_color[0] = 0.2f;
		buttonRefresh_.m_color[1] = 0.2f;
		buttonRefresh_.m_color[2] = 0.2f;
		StartRefresh();
	};

	buttonBack_.m_text = "Back";
	buttonBack_.m_color[0] = 0.4f;
	buttonBack_.m_color[1] = 0.4f;
	buttonBack_.m_color[2] = 0.4f;
	buttonBack_.m_pos = { windowWidth_ - 400.0f, windowHeight_ - 150.0f };
	buttonBack_.m_size = { 300, 80 };
	buttonBack_.hImage = hButton;
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
	if (refreshRequest_ != INVALID_HANDLE) {
		CancelRefreshRequest();
	}
}


void ServerBrowser::Update(float dt) {
	m_netManager.CheckConnected();
}


void ServerBrowser::Render(CRenderer* renderer) {
	if (refreshRequest_ != INVALID_HANDLE) {
		// we are refreshing!
		renderer->RenderText(370, 80, 150, 50, "Refreshing...");
	}

	renderer->RenderText(checkboxLan_.m_pos.x + 30, checkboxLan_.m_pos.y, 100, 20, "Lan servers");

	rootElement_.Render(renderer);
}

void ServerBrowser::MouseButtonPressed(int button) {
	rootElement_.MousePressed(mouseX_, mouseY_);
}

void ServerBrowser::StartRefresh() {
	bool bLan = checkboxLan_.IsSelected();

	if(bLan) {
		refreshRequest_ = m_netManager.StartServerSearch();
	}
	buttonRefresh_.m_text = "Cancel";
}

void ServerBrowser::RefreshComplete(void* hRequest, void* response) {
	printf("RefreshComplete");
	CancelRefreshRequest();
}

void ServerBrowser::ServerFailedToRespond(void* hRequest, int iServer) {
	printf("ServerFailedToRespond\n");
}

void ServerBrowser::CancelRefreshRequest() {
	refreshRequest_ = INVALID_HANDLE;
	buttonRefresh_.m_color[0] = 0.4f;
	buttonRefresh_.m_color[1] = 0.4f;
	buttonRefresh_.m_color[2] = 0.4f;
	buttonRefresh_.m_text = "Refresh";
}

void ServerBrowser::ServerResponded(void* hRequest, int iServer) {
	printf("ServerResponded\n");
}

void ServerBrowser::ServerFound(RequestResult_t result) {
	Server_t server{};
	server.addr = result.szIp;
	server.name = std::string(result.szIp);
	server.nPort = result.nPort;

	GuiServerElement* serverElement = new GuiServerElement(server);
	serverElement->e_onMousePressed = [this, serverElement]() {
		handler_->JoinLobby(serverElement->GetServer().addr, serverElement->GetServer().nPort);
	};
	serverElement->m_pos = { 50, 200 };
	serverElement->m_size = { 280, 30};
	rootElement_.m_children.push_back(serverElement);
}

void ServerBrowser::Action(EInputAction eAction) {
	if(eAction == EInputAction::MENU_CLOSE) {
		handler_->OpenMainMenu();
	}

	if(eAction == EInputAction::MENU_SELECT) {
		rootElement_.MousePressed(mouseX_, mouseY_);
	}
}