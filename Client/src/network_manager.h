#pragma once
#include <string>
#include "networking.h"

namespace PMG {
	class NetworkManager {
	public:
		bool Initialize();
		void ConnectToServer(std::string serverAddress, std::string port);
		bool IsConnected();
		bool Close();
		bool ReceivePacket(packet_t* packet);

		bool SendPacket(packet_t* packet);

	private:
		net_client_t connection_;
	};
}