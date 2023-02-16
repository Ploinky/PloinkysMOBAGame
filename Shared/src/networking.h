#pragma once

#include <string>
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <vector>

#define DEFAULT_PORT 23119


namespace PMG {
    enum class PacketType {
        UNITSPAWN,
        UNITMOVE,
        UNITIDLE,
        UNITDESPAWN,
        GAME_TICK,
    };


    typedef struct move_command_t {
        float nx;
        float ny;
    } move_command_t;

    typedef struct packet_header {
        PacketType type{};
        size_t size = 0;
    } packet_header_t;

    typedef struct packet {
        packet_header_t header;
        std::vector<uint8_t> data;

        size_t size() const {
            return sizeof(packet_header_t) + data.size();
        }

    } packet_t;

    template<typename DataType>
    packet_t& operator << (packet_t& packet, const DataType& data) {
        static_assert(std::is_standard_layout<DataType>::value, "Data too complex for packet");

        size_t i = packet.data.size();

        packet.data.resize(packet.data.size() + sizeof(DataType));

        std::memcpy(packet.data.data() + i, &data, sizeof(DataType));

        packet.header.size = packet.size();

        return packet;
    }

    template<typename DataType>
    packet_t& operator >> (packet_t& packet, DataType& data) {
        static_assert(std::is_standard_layout<DataType>::value, "Data too complex for packet");

        size_t i = packet.data.size() - sizeof(DataType);

        std::memcpy(&data, packet.data.data() + i, sizeof(DataType));

        packet.data.resize(i);

        packet.header.size = packet.size();

        return packet;
    }

    inline packet_t& operator << (packet_t& packet, packet_t& data) {
        size_t i = packet.data.size();

        packet.data.resize(packet.data.size() + data.size());

        std::memcpy(packet.data.data() + i, &data.header, sizeof(packet_header_t));
        std::memcpy(packet.data.data() + i + sizeof(packet_header_t), data.data.data(), data.header.size - sizeof(packet_header_t));

        packet.header.size = packet.size();

        return packet;
    }

    inline packet_t& operator >> (packet_t& packet, packet_t& data) {
        std::memcpy(&data.header, packet.data.data(), sizeof(packet_header_t));

        data.data.resize(data.header.size - sizeof(packet_header_t));

        std::memcpy(data.data.data(), packet.data.data() + sizeof(packet_header_t), data.header.size - sizeof(packet_header_t));

        packet.data.erase(packet.data.begin(), packet.data.begin() + data.header.size);

        packet.header.size = packet.size();

        return packet;
    }

  // Struct containing client networking data
  typedef struct {
    SOCKET socket;
    bool isConnected;
  } net_client_t;

  // Initialize networking
  bool Net_Init();

  // Connect to server, fill the supplied client structure with data
  bool Net_ConnectToServer(std::string serverAddress, std::string port, net_client_t* client);

  bool Net_CreateListenSocket(std::string port, net_client_t* listenServer);

  bool Net_AcceptConnection(net_client_t* listenServer, net_client_t* client);

  bool Net_ReceivePacket(net_client_t* connection, packet_t* packet);

  bool Net_SendPacket(packet_t* packet, net_client_t* connection);

  bool Net_CloseConnection(net_client_t* connection);

  bool Net_IsConnected(net_client_t* connection);

  // Clean up and tear down
  void Net_Shutdown();
}