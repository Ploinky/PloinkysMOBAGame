#pragma once
#include "steam/steam_api.h"
#include <string>
#include "networking.h"

namespace PMG {
	class NetworkManager {
	public:
		bool Initialize();
		void ConnectToServer(std::string ip);
		bool IsConnected();
		void Close();
		bool ReceivePacket(packet_t* packet);

		bool SendPacket(packet_t* packet);

		STEAM_CALLBACK(NetworkManager, OnConnectionStatusChangedCallback, SteamNetConnectionStatusChangedCallback_t);

	private:
		HSteamNetConnection connection_;
		bool is_connected_;
	};
}