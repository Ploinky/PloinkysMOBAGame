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

	m_boxServers.m_pos = {50, 200};
	m_boxServers.m_size = {windowWidth_ - 100.0f, windowHeight_ - 400.0f};
	m_boxServers.m_gap = 10;

	rootElement_.m_children.push_back(&buttonRefresh_);
	rootElement_.m_children.push_back(&buttonBack_);
	rootElement_.m_children.push_back(&checkboxLan_);
	rootElement_.m_children.push_back(&m_boxServers);
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
	server.name = result.szName;
	server.nPort = result.nPort;

	GuiServerElement* serverElement = new GuiServerElement(server);
	serverElement->e_onMousePressed = [this, serverElement]() {
		handler_->JoinLobby(serverElement->GetServer().addr, serverElement->GetServer().nPort);
	};
	m_boxServers.m_children.push_back(serverElement);
	m_boxServers.LayoutChildren();

	

	GuiServerElement* serverElement2 = new GuiServerElement(server);
	serverElement2->e_onMousePressed = [this, serverElement2]() {
		handler_->JoinLobby(serverElement2->GetServer().addr, serverElement2->GetServer().nPort);
	};
	m_boxServers.m_children.push_back(serverElement2);
	m_boxServers.LayoutChildren();

	

	GuiServerElement* serverElement3 = new GuiServerElement(server);
	serverElement3->e_onMousePressed = [this, serverElement3]() {
		handler_->JoinLobby(serverElement3->GetServer().addr, serverElement3->GetServer().nPort);
	};
	m_boxServers.m_children.push_back(serverElement3);
	m_boxServers.LayoutChildren();
}

void ServerBrowser::Action(EInputAction eAction) {
	if(eAction == EInputAction::MENU_CLOSE) {
		handler_->OpenMainMenu();
	}

	if(eAction == EInputAction::MENU_SELECT) {
		rootElement_.MousePressed(mouseX_, mouseY_);
	}
}