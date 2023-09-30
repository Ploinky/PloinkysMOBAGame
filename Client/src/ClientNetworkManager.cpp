#include "ClientNetworkManager.h"
#include "logger.h"
#include "pmg_networking.h"

#include "Client.h"

namespace PMG {
	bool ClientNetworkManager::IsConnected() {
		return connection_.isConnected;
	}

	bool ClientNetworkManager::Initialize(Networking::NetworkHandlerManager<Networking::PacketType>* manager) {
		WSADATA wsaData = {};

		int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (wsaStartupResult != 0) {
			return false;
		}

		this->packet_manager = manager;

		return true;
	}

	void ClientNetworkManager::ConnectToServer(std::string serverAddress, std::string port) {
		ADDRINFOA addrinfo = {};
		addrinfo.ai_family = AF_INET;
		addrinfo.ai_socktype = SOCK_STREAM;
		addrinfo.ai_protocol = IPPROTO_TCP;

		ADDRINFOA* addrResult = 0;

		int result = getaddrinfo(serverAddress.c_str(), port.c_str(), &addrinfo, &addrResult);

		if (result != 0) {
			return; // false;
		}

		SOCKET clientSocket = INVALID_SOCKET;

		clientSocket = socket(addrResult->ai_family, addrResult->ai_socktype, addrResult->ai_protocol);

		if (clientSocket == INVALID_SOCKET) {
			return; // false;
		}

		unsigned long nonblocking = 1;
		ioctlsocket(clientSocket, FIONBIO, &nonblocking);

		connection_.socket = clientSocket;
		connection_.isConnected = false;

		result = connect(clientSocket, addrResult->ai_addr, addrResult->ai_addrlen);

		if (result == SOCKET_ERROR) {
			return; // false;
		}

		freeaddrinfo(addrResult);
	}

	bool ClientNetworkManager::CheckConnected() {
		unsigned long mode = 1;

		union CSADDR_INFO_PADDED {
			CSADDR_INFO addr;
			char padding[128];
		} val {};

		int len = sizeof(val);

		int result = getsockopt(connection_.socket, SOL_SOCKET, SO_BSP_STATE, (char*) & val, &len);

		if (result == SOCKET_ERROR) {
			// failed to fetch connection status
			int err = WSAGetLastError();
			return false;
		}

		if (val.addr.RemoteAddr.iSockaddrLength == 0) {
			// not connected
			return false;
		}

		connection_.isConnected = true;

		return true;
	}

	bool ClientNetworkManager::Close() {
		int result = shutdown(connection_.socket, SD_BOTH);
		connection_.isConnected = false;

		if (result == SOCKET_ERROR) {
			return false;
		}

		return true;
	}

	bool ClientNetworkManager::SendPacket(Networking::BasePacket* packet) {
		std::vector<uint8_t> data;
		packet->Write(&data);

		int error = send(connection_.socket, (char*)data.data(), data.size(), 0);

		if (error < 1) {
			printf("failed sending <%d> with <%u> bytes to <%I64u>: %d\r\n",
				packet->type,
				data.size(),
				connection_.socket,
				WSAGetLastError()
			);
			Close();
			return false;
		}

		return true;
	}

	bool ClientNetworkManager::ReceivePacket() {
		Networking::packet_header_t header{};

		std::vector<uint8_t> data;
		int error = recv(connection_.socket, (char*)&header, sizeof(header), 0);

		if (error < 1) {
			return false;
		}

		data.resize(header.size);
		std::memcpy(data.data(), &header, sizeof(header));

		if (header.size > 8) {
			error = recv(connection_.socket, (char*)data.data() + sizeof(header), header.size - sizeof(header), 0);

			if (error < 1) {
				printf("failed receiving <%d> with <%u> bytes from <%I64u>: %d\r\n",
					header.type,
					data.size(),
					connection_.socket,
					WSAGetLastError()
				);
				return false;
			}
		}

		/*
		if (header.type == Networking::PacketType::GAME_TICK) {
			int data_index = sizeof(header);

			std::vector<uint8_t> tick_data;
			// read all packets
			while (data_index < data.size() - 4) {
				Networking::packet_header_t tick_packet_header{};
				std::memcpy(&tick_packet_header, data.data() + data_index, sizeof(tick_packet_header));
				data_index += sizeof(tick_packet_header);

				tick_data.resize(tick_packet_header.size);
				std::memcpy(tick_data.data(), &tick_packet_header, sizeof(tick_packet_header));
				if (tick_packet_header.size > sizeof(tick_packet_header)) {
					std::memcpy(tick_data.data() + sizeof(tick_packet_header), data.data(), tick_packet_header.size - sizeof(tick_packet_header));
					data_index += tick_packet_header.size - sizeof(tick_packet_header);
				}

				std::function fun = packet_manager->GetHandler(tick_packet_header.type);

				if (fun != nullptr) {
					fun(tick_data);
				}
			}

			// read game tick
			unsigned long game_tick;
			std::memcpy(&game_tick, data.data() + data_index, sizeof(unsigned long));
			data_index += sizeof(unsigned long);
		}
		*/

		std::function fun = packet_manager->GetHandler(header.type);

		if(fun != nullptr) {
			fun(data);
		}

		return true;
	}
}