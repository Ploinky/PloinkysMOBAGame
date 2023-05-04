#include "network_manager.h"
#include "logger.h"

namespace PMG {
	bool NetworkManager::Initialize() {
		SteamNetworkingUtils()->InitRelayNetworkAccess();
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
		connection_ = SteamNetworkingSockets()->ConnectByIPAddress(ipAddr, 0, 0);
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
			Logger::Msg("Connecting to server...");
		}
		else if (payload->m_info.m_eState == k_ESteamNetworkingConnectionState_ProblemDetectedLocally) {
			Logger::Msg("Could not connect to server.");
			Logger::Msg(std::to_string(payload->m_info.m_eEndReason));
			Logger::Msg(payload->m_info.m_szEndDebug);
		}
	}

	void NetworkManager::Close() {
		SteamNetworkingSockets()->CloseConnection(connection_, 0, 0, false);
	}

	bool NetworkManager::SendPacket(packet_t* packet) {
		EResult result = SteamNetworkingSockets()->SendMessageToConnection(connection_, &packet, sizeof(packet), 0, 0);
		return result == 0;
	}

	bool NetworkManager::ReceivePacket(packet_t* packet) {
		SteamNetworkingMessage_t** messages = 0;
		if (SteamNetworkingSockets()->ReceiveMessagesOnConnection(connection_, messages, 1)) {
			return messages[0]->GetData();
		}
	}
}