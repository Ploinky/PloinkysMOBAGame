#pragma once

#include <string>
#include <functional>
#include <vector>

#include "common/pmg_networking.h"
#include "common/PMG_Common.h"
#include "core/network/network-engine.h"

class Client;

class ClientNetworkManager {
public:
	bool Initialize(INetworkEngine* pEngine, NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>>* manager);

	HServerRequest StartServerSearch();
	void RegisterRequestObserver(IRequestObserver* pObserver);

	// Gameplay
	void ConnectToServer(std::string addr, int port);
	bool CheckConnected();
	bool IsConnected();
	bool Close();
	bool ReceivePacket();
	bool SendPacket(BasePacket* packet);

private:
	bool m_bIsConnectedToServer = false;
	HConnection m_hServerConnection = INVALID_HANDLE;
	NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>>* packet_manager;
	INetworkEngine* m_pEngine;
	std::vector<IRequestObserver*> m_vecObservers;
};
