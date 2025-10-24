#include "ClientNetworkManagerEnet.h"
#include <Common/PMG_Common.h>
#include <Common/pmg_networking.h>

#include "Game.h"

bool ClientNetworkManagerEnet::IsConnected() {
	return m_pServerConnection != nullptr && m_bIsConnectedToServer;
}

bool ClientNetworkManagerEnet::Initialize(NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>>* manager) {
    // TODO do we need to do anything here? like check for errors? 
    if(enet_initialize()) {
        Logger::FormatErr("Failed to initialize enet!");
        return false;
    }

	this->packet_manager = manager;
	return true;
}

void ClientNetworkManagerEnet::ConnectToServer(std::string addr) {
	m_pHost = enet_host_create (NULL /* create a client host */,
				1 /* only allow 1 outgoing connection */,
				2 /* allow up 2 channels to be used, 0 and 1 */,
				0 /* assume any amount of incoming bandwidth */,
				0 /* assume any amount of outgoing bandwidth */);
 
	if (m_pHost == NULL) {
		Logger::Err("An error occurred while trying to create an ENet client host.");
		return;
	}
	
	m_bIsConnectedToServer = false;

	ENetAddress address;
	ENetEvent event;
	ENetPeer *peer;
	
	enet_address_set_host (&address, addr.c_str());
	address.port = 23119;
	
	m_pServerConnection = enet_host_connect(m_pHost, &address, 2, 0);
	
	if (m_pServerConnection == NULL) {
		Logger::Err("Failed to connect to server: No available peers for initiating an ENet connection.");
		return;
	}
}

bool ClientNetworkManagerEnet::CheckConnected() {
	// TODO
	return true;
}

bool ClientNetworkManagerEnet::Close() {
    // TODO what is actually needed here? checks?
    enet_deinitialize();
	return true;
}

bool ClientNetworkManagerEnet::SendPacket(BasePacket* packet) {
	std::vector<uint8_t>* buf = new std::vector<uint8_t>();
	packet->Write(buf);

    ENetPacket* pPacket = enet_packet_create (buf->data(), buf->size(), ENET_PACKET_FLAG_RELIABLE);
 
    enet_peer_send (m_pServerConnection, 0, pPacket);
    enet_host_flush(m_pHost);

	return true;
}

bool ClientNetworkManagerEnet::ReceivePacket() {
    ENetEvent event;
	std::function<void (std::vector<uint8_t>)> fun = [](std::vector<uint8_t>){};
	std::vector<uint8_t> data;
	packet_header_t header{};

    while (enet_host_service(m_pHost, &event, 0) > 0) {
        switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                Logger::FormatMsg("A new client connected from %x:%u.\n", event.peer->address.host, event.peer->address.port);

                /* Store any relevant client information here. */
				m_bIsConnectedToServer = true;
				break;
            case ENET_EVENT_TYPE_RECEIVE:
				data.resize(event.packet->dataLength);
				std::memcpy(data.data(), event.packet->data, data.size());

				std::memcpy(&header, data.data(), sizeof(header));

				fun = packet_manager->GetHandler(header.type);

				if(fun != nullptr) {
					fun(data);
				}

                /* Clean up the packet now that we're done using it. */
                enet_packet_destroy (event.packet);

                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                printf ("%s disconnected.\n", event.peer -> data);
        
                /* Reset the peer's client information. */
				m_bIsConnectedToServer = false;
                break;
        }
        
    }
	return false;

	/*
	std::vector<uint8_t> data;

	std::vector<SteamNetworkingMessage_t*> messages;
	messages.resize(1);

	if (SteamNetworkingSockets()->ReceiveMessagesOnConnection(m_pServerConnection, messages.data(), 1)) {
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
	*/

}
