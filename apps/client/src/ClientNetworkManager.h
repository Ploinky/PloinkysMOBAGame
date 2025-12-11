#pragma once

#include <string>
#include <common/pmg_networking.h>
#include <common/PMG_Common.h>
#include <functional>

class Client;
typedef struct _ENetPeer ENetPeer;
typedef struct _ENetHost ENetHost;


class ClientNetworkManager {
public:
	bool Initialize(NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>>* manager);
	void ConnectToServer(std::string addr);
	bool CheckConnected();
	bool IsConnected();
	bool Close();
	bool ReceivePacket();

	bool SendPacket(BasePacket* packet);

private:
	bool m_bIsConnectedToServer = false;
	ENetPeer* m_pServerConnection;
	ENetHost* m_pHost;
	NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>>* packet_manager;
};
