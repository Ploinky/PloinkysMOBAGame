#include "ServerBrowser.h"
#include "Renderer.h"
#include <common/PMG_Common.h>

ServerBrowser::ServerBrowser(IClientStateHandler* handler, int width, int height) : IClientState(handler, width, height) {
	rootElement_.m_size = { static_cast<float>(windowWidth_), static_cast<float>(windowHeight_) };
	rootElement_.m_pos = { 0, 0 };
	HBitmap hButton = handler->GetAssetManager()->LoadBitmapImage("UI/Buttons/MenuButton/MenuButton.bmp");

	buttonRefresh_.m_text = "Refresh";
	buttonRefresh_.m_color[0] = 0.4f;
	buttonRefresh_.m_color[1] = 0.4f;
	buttonRefresh_.m_color[2] = 0.4f;
	buttonRefresh_.m_pos = { 50, 50 };
	buttonRefresh_.m_size = { 300, 80 };
	buttonRefresh_.hImage = hButton;
	buttonRefresh_.e_onMousePressed = [this]() {
		if (refreshRequest_) {
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
	if (refreshRequest_) {
		CancelRefreshRequest();
	}
}


void ServerBrowser::Update(float dt) {

}

void ServerBrowser::Render(CRenderer* renderer) {
	if (refreshRequest_) {
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
	// servers_.clear();

	MatchMakingKeyValuePair_t* filter = new MatchMakingKeyValuePair_t[]{ {} };

	if (checkboxLan_.IsSelected()) {
		refreshRequest_ = SteamMatchmakingServers()->RequestLANServerList(SteamUtils()->GetAppID(), this);
	}
	else {
		MatchMakingKeyValuePair_t** ppchFilters = new MatchMakingKeyValuePair_t * [0];
		refreshRequest_ = SteamMatchmakingServers()->RequestInternetServerList(SteamUtils()->GetAppID(), ppchFilters, 0, this);
	}

	buttonRefresh_.m_text = "Cancel";
	delete[] filter;
}

void ServerBrowser::RefreshComplete(HServerListRequest hRequest, EMatchMakingServerResponse response) {
	printf("RefreshComplete");
	if (hRequest != refreshRequest_) {
		// really this should not be happening i guess?
		SteamMatchmakingServers()->ReleaseRequest(hRequest);
	}
	CancelRefreshRequest();
}

void ServerBrowser::ServerFailedToRespond(HServerListRequest hRequest, int iServer) {
	printf("ServerFailedToRespond\n");
	gameserveritem_t* pServer = SteamMatchmakingServers()->GetServerDetails(hRequest, iServer);
	if (pServer)
	{
		// Filter out servers that don't match our appid here (might get these in LAN calls since we can't put more filters on it)
		if (pServer->m_nAppID == SteamUtils()->GetAppID())
		{
			SteamMatchmakingServers()->PingServer(pServer->m_NetAdr.GetIP(), 27016, this);
		}
	}
}

void ServerBrowser::CancelRefreshRequest() {
	SteamMatchmakingServers()->ReleaseRequest(refreshRequest_);
	refreshRequest_ = nullptr;
	buttonRefresh_.m_color[0] = 0.4f;
	buttonRefresh_.m_color[1] = 0.4f;
	buttonRefresh_.m_color[2] = 0.4f;
	buttonRefresh_.m_text = "Refresh";
}

void ServerBrowser::ServerResponded(HServerListRequest hRequest, int iServer) {
	printf("ServerResponded\n");

	gameserveritem_t* pServer = SteamMatchmakingServers()->GetServerDetails(hRequest, iServer);
	if (pServer)
	{
		// Filter out servers that don't match our appid here (might get these in LAN calls since we can't put more filters on it)
		if (pServer->m_nAppID == SteamUtils()->GetAppID())
		{
			Server_t server{};
			server.addr = pServer->m_NetAdr;
			server.name = std::string(pServer->GetName());

			GuiServerElement* serverElement = new GuiServerElement(server);
			serverElement->e_onMousePressed = [this, serverElement]() {
				handler_->JoinLobby(serverElement->GetServer().addr);
			};
			serverElement->m_pos = { 50, 200 };
			serverElement->m_size = { 280, 30};
			rootElement_.m_children.push_back(serverElement);
		}
	}
}