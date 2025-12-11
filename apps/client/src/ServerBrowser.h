#pragma once

#include "IClientState.h"
#include <string>
#include <vector>
#include "GuiButton.h"
#include "GuiServerElement.h"
#include "GuiCheckbox.h"
#include "Server_t.h"

class ServerBrowser : public IClientState {
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


	void* refreshRequest_ = nullptr;

	void CancelRefreshRequest();
	void StartRefresh();

	void RefreshComplete(void* hRequest, void* response);
	void ServerFailedToRespond(void* hRequest, int iServer);
	void ServerResponded(void* hRequest, int iServer);


	// Server has responded successfully and has updated data
	void ServerResponded(void* server) {
		printf("response\n");
	}

	// Server failed to respond to the ping request
	void ServerFailedToRespond() {
		printf("no response\n");
	}
};