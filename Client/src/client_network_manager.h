#pragma once
#include <string>
#include "networking.h"
#include "packets.h"
#include "pmg_networking.h"

namespace PMG {
	class Client;

	class ClientNetworkManager {
	public:
		bool Initialize(Networking::NetworkHandlerManager<PacketType>* manager);
		void ConnectToServer(std::string serverAddress, std::string port);
		bool CheckConnected();
		bool IsConnected();
		bool Close();
		bool ReceivePacket(packet_t* packet);

		bool SendPacket(packet_t* packet);

	private:
		net_client_t connection_;
		Networking::NetworkHandlerManager<PacketType>* packet_manager;
	};
}