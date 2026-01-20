#pragma once

#include "IClientState.h"
#include <string>
#include <vector>
#include "GuiButton.h"
#include "GuiServerElement.h"
#include "GuiCheckbox.h"
#include "Gui.h"
#include "Server_t.h"
#include "ClientNetworkManager.h"

// TODO move this out
#include <enet/enet.h>

class ServerBrowser : public IClientState, IRequestObserver {
public:
	ServerBrowser(IClientStateHandler* handler, int width, int height);
	~ServerBrowser();

	virtual void Update(float dt) override;
	virtual void Render(CRenderer* renderer) override;
	virtual void MouseButtonPressed(int button) override;

	virtual void Action(EInputAction eAction) override;
private:
	GuiElement rootElement_;
	GuiButton buttonRefresh_;
	GuiButton buttonBack_;
	GuiCheckbox checkboxLan_;
	VBox m_boxServers;

	HServerRequest refreshRequest_ = INVALID_HANDLE;

	void CancelRefreshRequest();
	void StartRefresh();

	void RefreshComplete(void* hRequest, void* response);
	void ServerFailedToRespond(void* hRequest, int iServer);
	void ServerResponded(void* hRequest, int iServer);
	virtual void ServerFound(RequestResult_t result) override;

	// Server has responded successfully and has updated data
	void ServerResponded(void* server) {
		printf("response\n");
	}

	// Server failed to respond to the ping request
	void ServerFailedToRespond() {
		printf("no response\n");
	}


	// TODO move this out
	std::unique_ptr<ENetHost> m_discoverHost = nullptr;
	ClientNetworkManager m_netManager;
};