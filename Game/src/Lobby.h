#pragma once

#include "IClientState.h"
#include <string>
#include "ClientNetworkManager.h"
#include <string>

namespace PMG {
	class Player {
	public:
		std::string name;
	};

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
		Player* players_[10]{ nullptr };
	};
}