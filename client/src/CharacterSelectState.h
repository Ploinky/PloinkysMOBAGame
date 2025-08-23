#pragma once

#include "IClientState.h"
#include <string>
#include "ClientNetworkManagerEnet.h"
#include <string>
#include "Gui.h"
#include <Common/NewPackets.h>
#include <Lobby.h>

class CCharacterSelectState : public IClientState {
public:
	CCharacterSelectState(ClientNetworkManagerEnet* server, IClientStateHandler* handler, int width, int height, Player** players);
	~CCharacterSelectState();

	virtual void Update(float dt) override;
	virtual void Render(CRenderer* renderer) override;
	virtual void MouseButtonPressed(int button) override;

private:
	NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>> packetManager_;
	ClientNetworkManagerEnet* networkManager_;

	void HandleCharacterSelectTimePacket(std::vector<uint8_t> data);

	int mySlot_;
	Player* players_[10]{ nullptr };

	GuiElement rootElement_;

	// TODO this should come from server
	int m_iRemainingTime = 10000;
};
