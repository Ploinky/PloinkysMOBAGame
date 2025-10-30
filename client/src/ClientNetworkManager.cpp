#include "ClientNetworkManagerEnet.h"
#include <Common/PMG_Common.h>
#include <Common/pmg_networking.h>

#include "Game.h"

#include "steam/isteamnetworkingsockets.h"

bool ClientNetworkManager::IsConnected() {
	return serverConnection_ != k_HSteamNetConnection_Invalid && m_bIsConnectedToServer;
}

bool ClientNetworkManager::Initialize(NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>>* manager) {
	this->packet_manager = manager;
	return true;
}

void ClientNetworkManager::ConnectToServer(std::string addr) {
	m_bIsConnectedToServer = false;
	SteamNetworkingIPAddr ipAddr{};
	ipAddr.ParseString(addr.c_str());
	SteamNetworkingConfigValue_t config{};
	config.SetInt32(k_ESteamNetworkingConfig_IP_AllowWithoutAuth, 1);
	serverConnection_ = SteamNetworkingSockets()->ConnectByIPAddress(ipAddr, 0, &config);

	if (serverConnection_ == k_HSteamNetConnection_Invalid) {
		return;
	}
}

void ClientNetworkManager::OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* callback) {
	Logger::Msg("Status changed");

	if (callback->m_info.m_eState == k_ESteamNetworkingConnectionState_Connected) {
		if (callback->m_eOldState != k_ESteamNetworkingConnectionState_Connected) {
			// TODO we are connected now but were not before
			m_bIsConnectedToServer = true;
		}
	}
	else if (callback->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
		if (callback->m_eOldState == k_ESteamNetworkingConnectionState_Connected) {
			// TODO we were connected but have become disconnected. Whoops!
			m_bIsConnectedToServer = false;
		}
	}

}

bool ClientNetworkManager::CheckConnected() {
	// TODO
	return true;
}

bool ClientNetworkManager::Close() {
	// TODO
	SteamNetworkingSockets()->CloseConnection(serverConnection_, 0, nullptr, false);

	return true;
}

bool ClientNetworkManager::SendPacket(BasePacket* packet) {
	std::vector<uint8_t>* buf = new std::vector<uint8_t>();
	packet->Write(buf);

	EResult result = SteamNetworkingSockets()->SendMessageToConnection(serverConnection_, buf->data(), buf->size(), 0, nullptr);

	if (result != k_EResultOK) {
		throw new std::exception();
	}

	return true;
}

bool ClientNetworkManager::ReceivePacket() {
	std::vector<uint8_t> data;

	std::vector<SteamNetworkingMessage_t*> messages;
	messages.resize(1);

	if (SteamNetworkingSockets()->ReceiveMessagesOnConnection(serverConnection_, messages.data(), 1)) {
		for (SteamNetworkingMessage_t* message : messages) {

			if (message == nullptr) {
				return false;
			}

			data.resize(message->GetSize());
			std::memcpy(data.data(), message->GetData(), message->GetSize());

			// release when done!
			message->Release();
		}
	}
	else {
		return false;
	}

	packet_header_t header{};
	std::memcpy(&header, data.data(), sizeof(header));

	std::function fun = packet_manager->GetHandler(header.type);

	if(fun != nullptr) {
		fun(data);
	}

	return true;
}
