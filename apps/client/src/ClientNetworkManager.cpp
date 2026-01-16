#include "ClientNetworkManager.h"
#include <Common/PMG_Common.h>
#include <Common/pmg_networking.h>

#define ENET_IMPLEMENTATION
#include "enet/enet.h"

#include "Game.h"

bool ClientNetworkManager::IsConnected() {
	return m_hServerConnection != INVALID_HANDLE && m_bIsConnectedToServer;
}

bool ClientNetworkManager::Initialize(INetworkEngine* pEngine, NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>>* manager) {
    // TODO do we need to do anything here? like check for errors? 
	this->m_pEngine = pEngine;
	this->packet_manager = manager;
	return true;
}


HServerRequest ClientNetworkManager::StartServerSearch() {
	return m_pEngine->RequestServers();
}

void ClientNetworkManager::RegisterRequestObserver(IRequestObserver* pObserver) {
	for(const auto& pExistingObserver : m_vecObservers) {
		if(pExistingObserver == pObserver) {
			return;
		}
	}

	m_vecObservers.push_back(pObserver);
	m_pEngine->RegisterRequestObserver(pObserver);
}

void ClientNetworkManager::ConnectToServer(std::string addr, int port) {
	m_hServerConnection = m_pEngine->ConnectToServer(addr.c_str(), port);
}

bool ClientNetworkManager::CheckConnected() {
	// TODO
	m_pEngine->Update(16);
	return true;
}

bool ClientNetworkManager::Close() {
    // TODO what is actually needed here? checks?
    enet_deinitialize();
	return true;
}

bool ClientNetworkManager::SendPacket(BasePacket* packet) {
	m_pEngine->SendMessageToConnection(m_hServerConnection, packet);
	return true;
}

bool ClientNetworkManager::ReceivePacket() {
	bool bEvt = false;

	CNetworkEvent evt;
	while(m_pEngine->PollConnection(m_hServerConnection, &evt)) {
		bEvt = true;

		if(evt.type == ENetworkEventType::CONNECTED) {
			m_bIsConnectedToServer = true;
		} else if(evt.type == ENetworkEventType::PACKET_RECEIVED) {
			packet_header_t header{};
			std::memcpy(&header, evt.data.data(), sizeof(header));
			std::function<void (std::vector<uint8_t>)> fun = packet_manager->GetHandler(header.type);
			if(fun != nullptr) {
				fun(evt.data);
			}
		} else if(evt.type == ENetworkEventType::DISCONNECTED) {
			m_bIsConnectedToServer = false;
		} else if(evt.type == ENetworkEventType::DISCONNECTED_TIMEOUT) {
			m_bIsConnectedToServer = false;
		}
	}

	return bEvt;
}