#pragma once
#include <string>
#include "networking.h"
#include "pmg_networking.h"

namespace PMG {
	class Client;

	class ClientNetworkManager {
	public:
		bool Initialize(Networking::NetworkHandlerManager<Networking::PacketType>* manager);
		void ConnectToServer(std::string serverAddress, std::string port);
		bool CheckConnected();
		bool IsConnected();
		bool Close();
		bool ReceivePacket();

		bool SendPacket(Networking::BasePacket* packet);

	private:
		net_client_t connection_;
		Networking::NetworkHandlerManager<Networking::PacketType>* packet_manager;
	};
}