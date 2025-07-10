#pragma once
#include <string>
#include <common/pmg_networking.h>
#include <Common/PMG_Common.h>
#include <steam/steam_api.h>
#include <functional>

class Client;

class ClientNetworkManager {
public:
	bool Initialize(NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>>* manager);
	void ConnectToServer(std::string addr);
	bool CheckConnected();
	bool IsConnected();
	bool Close();
	bool ReceivePacket();

	bool SendPacket(BasePacket* packet);
	STEAM_CALLBACK(ClientNetworkManager, OnConnectionStatusChanged, SteamNetConnectionStatusChangedCallback_t);

private:
	bool m_bIsConnectedToServer = false;
	HSteamNetConnection serverConnection_;
	NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>>* packet_manager;
};
