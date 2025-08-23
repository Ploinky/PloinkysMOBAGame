#pragma once

#include "IClientState.h"
#include <string>
#include "ClientNetworkManagerEnet.h"
#include <string>
#include "Gui.h"
#include <Common/NewPackets.h>
#include <Player.h>

class Lobby : public IClientState {
public:
	Lobby(std::string server, IClientStateHandler* handler, int width, int height);
	~Lobby();
	virtual void Update(float dt) override;
	virtual void Render(CRenderer* renderer) override;
	virtual void MouseButtonPressed(int button) override;

private:
	NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>> packetManager_;
	ClientNetworkManagerEnet* networkManager_;

	void HandleSlotPacket(std::vector<uint8_t> data);
	void HandleGameStartPacket(std::vector<uint8_t> data);
	void HandleCharacterSelectTimePacket(std::vector<uint8_t> data);

	int mySlot_;
	Player* players_[10]{ nullptr };

	GuiElement rootElement_;
};
