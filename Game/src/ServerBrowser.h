#pragma once

#include "IClientState.h"
#include <string>
#include <vector>
#include "GuiButton.h"
#include "steam/isteammatchmaking.h"
#include "GuiCheckbox.h"

namespace PMG {
	struct Server_t {
		std::string name;
		servernetadr_t addr;
	};

	class ServerBrowser : public IClientState, public ISteamMatchmakingServerListResponse, ISteamMatchmakingPingResponse {
	public:
		ServerBrowser(IClientStateHandler* handler, int width, int height);
		~ServerBrowser();

		virtual void Update(float dt) override;
		virtual void Render(Renderer* renderer) override;
		virtual void MouseButtonPressed(int button) override;

	private:
		GuiButton buttonRefresh_;
		GuiButton buttonBack_;
		GuiCheckbox checkboxLan_;


		HServerListRequest refreshRequest_ = nullptr;

		std::vector<Server_t> servers_;

		void CancelRefreshRequest();
		void StartRefresh();

		void RefreshComplete(HServerListRequest hRequest, EMatchMakingServerResponse response) override;
		void ServerFailedToRespond(HServerListRequest hRequest, int iServer) override;
		void ServerResponded(HServerListRequest hRequest, int iServer) override;


		// Server has responded successfully and has updated data
		void ServerResponded(gameserveritem_t& server) {
			printf("response\n");
		}

		// Server failed to respond to the ping request
		void ServerFailedToRespond() {
			printf("no response\n");
		}
	};
}