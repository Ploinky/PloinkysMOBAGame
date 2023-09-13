#include "client_network_manager.h"
#include "logger.h"
#include "pmg_networking.h"

#include "client.h"

namespace PMG {
	bool ClientNetworkManager::IsConnected() {
		return connection_.isConnected;
	}

	bool ClientNetworkManager::Initialize(Networking::NetworkHandlerManager<PacketType>* manager) {
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

	bool ClientNetworkManager::SendPacket(packet_t* packet) {
		size_t sendBufLen = packet->size();
		char* sendBuf = (char*)std::malloc(sendBufLen);

		if (sendBuf == 0) {
			return false;
		}

		std::memcpy(sendBuf, &packet->header, sizeof(packet_header_t));
		std::memcpy(&sendBuf[sizeof(packet_header_t)], packet->data.data(), packet->size() - sizeof(packet_header_t));
		int error = send(connection_.socket, sendBuf, sendBufLen, 0);

		if (error < 1) {
			printf("failed sending <%d> with <%I64u> bytes to <%I64u>: %d\r\n",
				packet->header.type,
				packet->size(),
				connection_.socket,
				WSAGetLastError()
			);
			free(sendBuf);
			Close();
			return false;
		}

		free(sendBuf);

		return true;
	}

	bool ClientNetworkManager::ReceivePacket(packet_t* packet) {
		int error = recv(connection_.socket, (char*)&packet->header, sizeof(packet_header_t), 0);

		if (error < 1) {
			return false;
		}

		packet->data.resize(packet->header.size - sizeof(packet_header_t));

		if (packet->header.size > 8) {
			error = recv(connection_.socket, (char*)packet->data.data(), packet->header.size - sizeof(packet_header_t), 0);

			if (error < 1) {
				printf("failed receiving <%d> with <%I64u> bytes from <%I64u>: %d\r\n",
					packet->header.type,
					packet->size(),
					connection_.socket,
					WSAGetLastError()
				);
				return false;
			}
		}

		std::function fun = packet_manager->GetHandler(packet->header.type);

		if(fun != nullptr) {
			fun(packet->data);
		}

		return true;
	}
}