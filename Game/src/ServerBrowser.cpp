#include "ServerBrowser.h"
#include "Renderer.h"
#include "util.h"

namespace PMG {
	ServerBrowser::ServerBrowser(IClientStateHandler* handler, int width, int height) : IClientState(handler, width, height) {
		buttonRefresh_.m_text = "Refresh";
		buttonRefresh_.m_color[0] = 0.4f;
		buttonRefresh_.m_color[1] = 0.4f;
		buttonRefresh_.m_color[2] = 0.4f;
		buttonRefresh_.m_pos = { 50, 50 };
		buttonRefresh_.m_size = { 300, 80 };
		buttonRefresh_.e_onButtonPressed = [this]() {
			buttonRefresh_.m_color[0] = 0.2;
			buttonRefresh_.m_color[1] = 0.2;
			buttonRefresh_.m_color[2] = 0.2;
			StartRefresh();
		};

		buttonBack_.m_text = "Back";
		buttonBack_.m_color[0] = 0.4f;
		buttonBack_.m_color[1] = 0.4f;
		buttonBack_.m_color[2] = 0.4f;
		buttonBack_.m_pos = { windowWidth_ - 400.0f, windowHeight_ - 150.0f };
		buttonBack_.m_size = { 300, 80 };
		buttonBack_.e_onButtonPressed = [this]() {
			handler_->OpenMainMenu();
		};
	}

	ServerBrowser::~ServerBrowser() {
		if (refreshRequest_) {
			CancelRefreshRequest();
		}
	}


	void ServerBrowser::Update(float dt) {

	}

	void ServerBrowser::Render(Renderer* renderer) {
		buttonRefresh_.Render(renderer);
		float color[3]{ 1, 0, 0 };

		for (int i = 0; i < servers_.size(); i++) {
			renderer->RenderText(50, 200 + i * 50, 200, 30, color, std::string(servers_[i].name));
			renderer->RenderText(250, 200 + i * 50, 200, 30, color, std::string(servers_[i].addr.GetConnectionAddressString()));
		}

		if (refreshRequest_) {
			// we are refreshing!
			renderer->RenderText(370, 80, 150, 50, "Refreshing...");
		}

		buttonBack_.Render(renderer);
	}

	void ServerBrowser::MouseButtonPressed(int button) {
		if (mouseX_ >= buttonRefresh_.m_pos.x && mouseX_ <= buttonRefresh_.m_pos.x + buttonRefresh_.m_size.x
			&& mouseY_ >= buttonRefresh_.m_pos.y && mouseY_ <= buttonRefresh_.m_pos.y + buttonRefresh_.m_size.y) {
			buttonRefresh_.MousePressed(mouseX_, mouseY_);
		}

		int index = 0;
		for (Server_t addr : servers_) {
			if (mouseX_ >= 50 && mouseX_ <= 250
				&& mouseY_ >= 200 + index * 50 && mouseY_ <= 200 + index * 50 + 30) {
				handler_->JoinGame(addr.addr);
			}
		}

		// TODO i do not want to do this every time for every button yikes
		if (mouseX_ >= buttonBack_.m_pos.x && mouseX_ <= buttonBack_.m_pos.x + buttonBack_.m_size.x
			&& mouseY_ >= buttonBack_.m_pos.y && mouseY_ <= buttonBack_.m_pos.y + buttonBack_.m_size.y) {
			buttonBack_.MousePressed(mouseX_, mouseY_);
		}
	}

	void ServerBrowser::StartRefresh() {
		servers_.clear();

		MatchMakingKeyValuePair_t* filter = new MatchMakingKeyValuePair_t[]{ {} };
		refreshRequest_ = SteamMatchmakingServers()->RequestLANServerList(1756910, this);
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
	}

	void ServerBrowser::CancelRefreshRequest() {
		SteamMatchmakingServers()->ReleaseRequest(refreshRequest_);
		refreshRequest_ = nullptr;
		buttonRefresh_.m_color[0] = 0.4f;
		buttonRefresh_.m_color[1] = 0.4f;
		buttonRefresh_.m_color[2] = 0.4f;
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
				servers_.push_back(server);
			}
		}
	}
}