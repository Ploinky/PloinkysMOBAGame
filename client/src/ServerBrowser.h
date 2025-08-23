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

private:
	GuiElement rootElement_;
	GuiButton buttonRefresh_;
	GuiButton buttonBack_;
	GuiCheckbox checkboxLan_;

	void CancelRefreshRequest();
	void StartRefresh();
};
