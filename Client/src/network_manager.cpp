#include "network_manager.h"
#include "logger.h"

namespace PMG {
	bool NetworkManager::Initialize() {
		connection_ = false;
		is_connected_ = false;
		return true;
	}

	bool NetworkManager::IsConnected() {
		return is_connected_;
	}

	void NetworkManager::ConnectToServer(std::string ip) {
		SteamNetworkingIdentity host{};
		SteamNetworkingIPAddr ipAddr{};
		ipAddr.ParseString(ip.c_str());
		host.SetIPAddr(ipAddr);

		SteamNetworkingConfigValue_t config{};
		config.SetInt32(k_ESteamNetworkingConfig_IP_AllowWithoutAuth, 1);
		connection_ = SteamNetworkingSockets()->ConnectByIPAddress(ipAddr, 1, &config);
	}

	void NetworkManager::OnConnectionStatusChangedCallback(SteamNetConnectionStatusChangedCallback_t* payload) {
		if (payload->m_eOldState == k_ESteamNetworkingConnectionState_Connecting
			&& payload->m_info.m_eState == k_ESteamNetworkingConnectionState_Connected) {
			Logger::Msg("Connection status changed, server accepted connection...");
			is_connected_ = true;
		}
		else if(payload->m_info.m_eState == k_ESteamNetworkingConnectionState_Connecting) {
			Logger::Msg("Connecting to server...");
		}
		else if (payload->m_info.m_eState == k_ESteamNetworkingConnectionState_Dead) {
			Logger::Msg("Connection to server failed...");
		}
		else if (payload->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
			Logger::Msg("Could not connect to server.");
			Logger::Msg(std::to_string(payload->m_info.m_eEndReason));
			Logger::Msg(payload->m_info.m_szEndDebug);
		}
	}

	void NetworkManager::OnSteamNetAuthenticationStatus(SteamNetAuthenticationStatus_t* payload) {
		if (payload->m_eAvail) {
			Logger::Msg("Authentication available");
		}
		else {
			Logger::Msg("Authentication not available");
			Logger::Msg(payload->m_debugMsg);
		}
	}

	void NetworkManager::Close() {
		SteamNetworkingSockets()->CloseConnection(connection_, 0, 0, false);
	}

	bool NetworkManager::SendPacket(packet_t* packet) {
		EResult result = SteamNetworkingSockets()->SendMessageToConnection(connection_, packet, static_cast<uint32>(packet->size()), 0, 0);
		return result == 0;
	}

	bool NetworkManager::ReceivePacket(packet_t* packet) {
		SteamNetworkingMessage_t* messages[1] = { 0 };
		if (SteamNetworkingSockets()->ReceiveMessagesOnConnection(connection_, messages, 1) == 1 && messages[0] != 0) {
			size_t size = messages[0]->GetSize();
			memcpy(&packet->header, messages[0]->GetData(), sizeof(packet_header_t));
			Logger::Msg(std::to_string((uint32) packet->header.type));
			Logger::Msg(std::to_string(packet->header.size));
			packet->data.resize(packet->header.size - sizeof(packet_header_t));
			memcpy(packet->data.data(), ((char*)messages[0]->GetData()) + sizeof(packet_header_t), size - sizeof(packet_header_t));
			messages[0]->Release();
			return true;
		}
		return false;
	}
}