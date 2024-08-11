#include "ClientNetworkManager.h"
#include <Common/PMG_Common.h>
#include <Common/pmg_networking.h>

#include "Game.h"

#include "steam/isteamnetworkingsockets.h"

namespace PMG {
	bool ServerNetworkManager::IsConnected() {
		return serverConnection_ != k_HSteamNetConnection_Invalid;
	}

	bool ServerNetworkManager::Initialize(Networking::NetworkHandlerManager<Networking::PacketType, std::function<void(std::vector<uint8_t>)>>* manager) {
		this->packet_manager = manager;
		return true;
	}

	void ServerNetworkManager::ConnectToServer(std::string addr) {
		SteamNetworkingIPAddr ipAddr{};
		ipAddr.ParseString(addr.c_str());
		SteamNetworkingConfigValue_t config{};
		config.SetInt32(k_ESteamNetworkingConfig_IP_AllowWithoutAuth, 1);
		serverConnection_ = SteamNetworkingSockets()->ConnectByIPAddress(ipAddr, 0, &config);

		if (serverConnection_ == k_HSteamNetConnection_Invalid) {
			return;
		}
	}

	void ServerNetworkManager::OnConnectionStatusChanged(SteamNetConnectionStatusChangedCallback_t* callback) {
		Logger::Msg("Status changed");
	}

	bool ServerNetworkManager::CheckConnected() {
		// TODO
		return true;
	}

	bool ServerNetworkManager::Close() {
		// TODO
		SteamNetworkingSockets()->CloseConnection(serverConnection_, 0, nullptr, false);

		return true;
	}

	bool ServerNetworkManager::SendPacket(Networking::BasePacket* packet) {
		std::vector<uint8_t>* buf = new std::vector<uint8_t>();
		packet->Write(buf);

		EResult result = SteamNetworkingSockets()->SendMessageToConnection(serverConnection_, buf->data(), buf->size(), 0, nullptr);

		if (result != k_EResultOK) {
			throw new std::exception();
		}

		return true;
	}

	bool ServerNetworkManager::ReceivePacket() {
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

		Networking::packet_header_t header{};
		std::memcpy(&header, data.data(), sizeof(header));

		std::function fun = packet_manager->GetHandler(header.type);

		if(fun != nullptr) {
			fun(data);
		}

		return true;
	}
}