#pragma once

#include "IClientState.h"
#include <string>
#include <vector>
#include "GuiButton.h"
#include "steam/isteammatchmaking.h"

namespace PMG {
	struct Server_t {
		std::string name;
		servernetadr_t addr;
	};

	class ServerBrowser : public IClientState, public ISteamMatchmakingServerListResponse {
	public:
		ServerBrowser(IClientStateHandler* handler, int width, int height);
		~ServerBrowser();

		virtual void Update(float dt) override;
		virtual void Render(Renderer* renderer) override;
		virtual void MouseButtonPressed(int button) override;

	private:
		GuiButton buttonRefresh_;
		GuiButton buttonBack_;


		HServerListRequest refreshRequest_ = nullptr;

		std::vector<Server_t> servers_;

		void CancelRefreshRequest();
		void StartRefresh();

		void RefreshComplete(HServerListRequest hRequest, EMatchMakingServerResponse response) override;
		void ServerFailedToRespond(HServerListRequest hRequest, int iServer) override;
		void ServerResponded(HServerListRequest hRequest, int iServer) override;
	};
}