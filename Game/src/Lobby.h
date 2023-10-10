#pragma once

#include "IClientState.h"
#include <string>
#include "ClientNetworkManager.h"

namespace PMG {
	class Lobby : public IClientState {
	public:
		Lobby(std::string server, IClientStateHandler* handler, int width, int height);
		virtual void Update(float dt) override;
		virtual void Render(Renderer* renderer) override;
		virtual void MouseButtonPressed(int button) override;

	private:
		Networking::NetworkHandlerManager<Networking::PacketType> packetManager_;
		ServerNetworkManager networkManager_;

		void HandleSlotPacket(std::vector<uint8_t> data);

		int mySlot_;
	};
}