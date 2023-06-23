#include "network_manager.h"
#include "logger.h"

namespace PMG {
	bool NetworkManager::IsConnected() {
		return connection_.isConnected;
	}

	bool NetworkManager::Initialize() {
		WSADATA wsaData = {};

		int wsaStartupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (wsaStartupResult != 0) {
			return false;
		}

		return true;
	}

	void NetworkManager::ConnectToServer(std::string serverAddress, std::string port) {
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

		result = connect(clientSocket, addrResult->ai_addr, addrResult->ai_addrlen);

		if (result == SOCKET_ERROR) {
			return; // false;
		}

		freeaddrinfo(addrResult);

		unsigned long mode = 1;

		result = ioctlsocket(clientSocket, FIONBIO, &mode);

		if (result == SOCKET_ERROR) {
			return;  // false;
		}

		connection_.socket = clientSocket;
		connection_.isConnected = true;

		return; // true;
	}

	bool NetworkManager::Close() {
		int result = shutdown(connection_.socket, SD_BOTH);
		connection_.isConnected = false;

		if (result == SOCKET_ERROR) {
			return false;
		}

		return true;
	}

	bool NetworkManager::SendPacket(packet_t* packet) {
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

	bool NetworkManager::ReceivePacket(packet_t* packet) {
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

		return true;
	}
}