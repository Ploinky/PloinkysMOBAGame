#pragma once
#include <string>
#include "networking.h"
#include "pmg_networking.h"
#include "steam\steam_api.h"

namespace PMG {
	class Client;

	class ServerNetworkManager {
	public:
		bool Initialize(Networking::NetworkHandlerManager<Networking::PacketType>* manager);
		void ConnectToServer(std::string addr);
		bool CheckConnected();
		bool IsConnected();
		bool Close();
		bool ReceivePacket();

		bool SendPacket(Networking::BasePacket* packet);
		STEAM_CALLBACK(ServerNetworkManager, OnConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t);

	private:
		net_client_t connection_;
		HSteamNetConnection serverConnection_;
		Networking::NetworkHandlerManager<Networking::PacketType>* packet_manager;

	};
}