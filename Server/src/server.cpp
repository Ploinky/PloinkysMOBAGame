#include <chrono>
#include "server.h"
#include "network_manager.h"
#include "main.h"
#include "game.h"
#include "util.h"
#include "logger.h"
#include "spell.h"

namespace PMG {
    void Server::Start() {
        m_networkManager = new ClientNetworkManager();
        m_networkManager->Initialize();
        m_networkManager->on_clientConnected = std::bind(&Server::OnClientConnected, this, std::placeholders::_1);
        m_networkManager->on_clientDisconnected = std::bind(&Server::OnClientDisconnected, this, std::placeholders::_1);
        m_networkManager->on_clientMessageReceived = std::bind(&Server::OnMessageReceived, this, std::placeholders::_1, std::placeholders::_2);
        if (!m_networkManager->CreateListenSocket("23119")) {
            Logger::Msg("Failed to create listen socket");
            return;
        }

        m_game = new Game();
        m_game->on_batchSendToAllClients = std::bind(&Server::BroadcastMessage, this, std::placeholders::_1);
        m_game->on_sendToClient = std::bind(&Server::SendMessageToClient, this, std::placeholders::_1, std::placeholders::_2);
        m_game->on_sendNewToClient = std::bind(&Server::SendNewMessageToClient, this, std::placeholders::_1, std::placeholders::_2);
        m_game->on_sendToAllClients = std::bind(&Server::SendMessageToAllClients, this, std::placeholders::_1);
        m_game->Start();

        long long lastFrame = GetSystemTime();

        bool isRunning = true;

        Logger::Msg("Server started");

        while(isRunning) {
            auto thisFrame = GetSystemTime();
            float dt = (thisFrame - lastFrame) / 1000000.0f / 1000.0f;
            lastFrame = thisFrame;

            m_networkManager->Update();

            m_game->Update(dt);
        }

        m_networkManager->Close();
        delete m_networkManager;
        m_networkManager = NULL;
    }

    long long Server::GetSystemTime() {
        return std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }

    void Server::OnClientConnected(unsigned long id) {
        printf("New client connected, id: %ld\r\n", id);
        m_game->AddPlayerForNetworkId(id);
    }

    void Server::OnClientDisconnected(unsigned long id) {
        printf("Client %ld disconnected\r\n", id);
        m_game->RemovePlayerForNetworkId(id);
    }

    void Server::OnMessageReceived(unsigned long clientId, packet_t* packet) {
        switch (packet->header.type) {
            case PacketType::UNITMOVE: {
                std::vector<uint8_t> new_data;
                new_data.resize(packet->header.size);
                std::memcpy(new_data.data(), &packet->header, sizeof(packet_header_t));
                std::memcpy(new_data.data() + sizeof(packet_header_t), packet->data.data(), packet->header.size - sizeof(packet_header_t));

                Networking::MoveCommandPacket move_command = Networking::MoveCommandPacket();
                move_command.Read(&new_data);

                m_game->PlayerMoveCommand(clientId, move_command.x, move_command.y);
                break;
            }
            case PacketType::CMD_STOP: {
                std::vector<uint8_t> new_data;
                new_data.resize(packet->header.size);
                std::memcpy(new_data.data(), &packet->header, sizeof(packet_header_t));
                std::memcpy(new_data.data() + sizeof(packet_header_t), packet->data.data(), packet->header.size - sizeof(packet_header_t));

                Networking::StopCommandPacket move_command = Networking::StopCommandPacket();
                move_command.Read(&new_data);

                m_game->PlayerStopCommand(clientId);
                break;
            }
            case PacketType::CMD_ATTACK: {
                std::vector<uint8_t> new_data;
                new_data.resize(packet->header.size);
                std::memcpy(new_data.data(), &packet->header, sizeof(packet_header_t));
                std::memcpy(new_data.data() + sizeof(packet_header_t), packet->data.data(), packet->header.size - sizeof(packet_header_t));

                Networking::AttackCommandPacket atk_command = Networking::AttackCommandPacket();
                atk_command.Read(&new_data);

                m_game->PlayerAttackCommand(clientId, atk_command.target_unit);
                break;
            }
            case PacketType::CMD_CAST: {
                cmd_cast_t cast{};
                *packet >> cast;

                SpellTargetInfo* target_info = new SpellTargetInfo();
                target_info->target_point = { cast.x, cast.y, cast.z };
                m_game->PlayerCastSpellCommand(clientId, cast.spell_slot, target_info);
                break;
            }
            case PacketType::CMD_CAST_TARGET: {
                cmd_cast_target_t cast{};
                *packet >> cast;

                SpellTargetInfo* target_info = new SpellTargetInfo();
                target_info->target = m_game->GetGameObjectById(cast.target);
                m_game->PlayerCastSpellCommand(clientId, cast.spell_slot, target_info);
                break;
            }
        }
    }

    void Server::BroadcastMessage(std::vector<packet_t> packet) {
        for(auto p : packet) {
            m_networkManager->SendToAllClients(&p);
        }
    }

    void Server::SendMessageToClient(unsigned long clientId, packet_t* packet) {
        m_networkManager->SendToClient(clientId, packet);
    }
    void Server::SendNewMessageToClient(unsigned long clientId, Networking::BasePacket* packet) {
        m_networkManager->SendToClient(clientId, packet);
    }

    void Server::SendMessageToAllClients(packet_t* packet) {
        m_networkManager->SendToAllClients(packet);
    }
}