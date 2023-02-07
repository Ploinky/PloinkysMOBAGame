#include "network_manager.h"

namespace PMG {
    NetworkManager::NetworkManager() {
        m_isHosting = false;

        Net_Init();
    }

  void NetworkManager::Host() {
    m_networkServer = {};
    Net_CreateListenSocket(std::to_string(DEFAULT_PORT), &m_networkServer);
  }

  void NetworkManager::Close() {
    m_isHosting = false;

    Net_CloseConnection(&m_networkServer);
  }

  void NetworkManager::Update() {
    // Currently allowing new connections at any time
    if (m_networkServer.isConnected) {
        net_client_t newClient = {};

      if(Net_AcceptConnection(&m_networkServer, &newClient)) {
        clients.push_back(newClient);
        on_clientConnected(newClient.socket);
      }

      for (auto it = clients.begin(); it != clients.end(); ++it) {
        net_client_t client = *it;
        packet_t packet = {};

        while(Net_ReceivePacket(&client, &packet)) {
          on_clientMessageReceived(client.socket, &packet);
        }
      }

      for (auto it = clients.begin(); it != clients.end(); ++it) {
        net_client_t client = *it;

        if(!Net_IsConnected(&client)) {
          it = clients.erase(it);
          on_clientDisconnected(client.socket);
          if (it == clients.end()) {
              break;
          }
        }
      }
    }
  }

  void NetworkManager::SendToAllClients(packet_t* packet) {
    for (auto it = clients.begin(); it != clients.end(); ++it) {
      net_client_t client = *it;

      if(client.isConnected) {
        Net_SendPacket(packet, &client);
      }
    }
  }

  void NetworkManager::SendToClient(unsigned long id, packet_t* packet) {
    for (auto it = clients.begin(); it != clients.end(); ++it) {
      net_client_t client = *it;

      if (client.socket == id) {
        if(client.isConnected) {
          Net_SendPacket(packet, &client);
        }
      }
    }
  }
}