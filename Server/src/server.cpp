#include <chrono>
#include "server.h"
#include "NetworkManager.h"
#include "main.h"
#include "game.h"
#include "util.h"
#include "logger.h"
#include "spell.h"
#include "attackable.h"
#include "steam/steam_gameserver.h"
#include "steam/steam_api.h"

#include "LobbyState.h"

namespace PMG {
    void Server::Start() {
        LobbyState state = LobbyState();


        bool isRunning = true;

        while (isRunning) {
            state.Update(0);

            SteamGameServer_RunCallbacks();
        }

        m_networkManager = new ServerNetworkManager();
        m_networkManager->Initialize();
        m_networkManager->on_clientConnected = std::bind(&Server::OnClientConnected, this, std::placeholders::_1);
        m_networkManager->on_clientDisconnected = std::bind(&Server::OnClientDisconnected, this, std::placeholders::_1);
        m_networkManager->on_clientMessageReceived = std::bind(&Server::OnMessageReceived, this, std::placeholders::_1, std::placeholders::_2);
        if (!m_networkManager->CreateListenSocket("23119")) {
            Logger::Msg("Failed to create listen socket");
            return;
        }

        m_game = new Client();
        m_game->on_sendToClient = std::bind(&Server::SendMessageToClient, this, std::placeholders::_1, std::placeholders::_2);
        m_game->on_sendToAllClients = std::bind(&Server::SendMessageToAllClients, this, std::placeholders::_1);
        m_game->Start();

        long long lastFrame = GetSystemTime();

        // TODO
        // bool isRunning = true;

        Logger::Msg("Server started");

        while(isRunning) {
            auto thisFrame = GetSystemTime();
            float dt = (thisFrame - lastFrame) / 1000000.0f / 1000.0f;
            lastFrame = thisFrame;

            m_networkManager->Update();

            m_game->Update(dt);

            SteamGameServer_RunCallbacks();
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

    void Server::OnMessageReceived(unsigned long clientId, std::vector<uint8_t>* data) {
        Networking::packet_header_t header{};
        std::memcpy(&header, data->data(), sizeof(header));

        switch (header.type) {
            case Networking::PacketType::UNITMOVE: {
                Networking::MoveCommandPacket move_command = Networking::MoveCommandPacket();
                move_command.Read(data);

                m_game->PlayerMoveCommand(clientId, move_command.x, move_command.y);
                break;
            }
            case Networking::PacketType::CMD_STOP: {
                Networking::StopCommandPacket move_command = Networking::StopCommandPacket();
                move_command.Read(data);

                m_game->PlayerStopCommand(clientId);
                break;
            }
            case Networking::PacketType::CMD_ATTACK: {
                Networking::AttackCommandPacket atk_command = Networking::AttackCommandPacket();
                atk_command.Read(data);

                m_game->PlayerAttackCommand(clientId, atk_command.target_unit);
                break;
            }
            case Networking::PacketType::CMD_CAST: {
                Networking::CastCommandPacket cast = Networking::CastCommandPacket();
                cast.Read(data);


                SpellTargetInfo* target_info = new SpellTargetInfo();
                target_info->target_point = { cast.x, cast.y, cast.z };
                m_game->PlayerCastSpellCommand(clientId, cast.spell_slot, target_info);
                break;
            }
            case Networking::PacketType::CMD_CAST_TARGET: {
                Networking::CastTargetCommandPacket cast_command = Networking::CastTargetCommandPacket();
                cast_command.Read(data);

                SpellTargetInfo* target_info = new SpellTargetInfo();
                target_info->target = dynamic_cast<Attackable*>(m_game->GetGameObjectById(cast_command.target));
                m_game->PlayerCastSpellCommand(clientId, cast_command.spell_slot, target_info);
                break;
            }
        }
    }

    void Server::SendMessageToClient(unsigned long clientId, std::vector<uint8_t>* packet) {
        m_networkManager->SendToClient(clientId, packet);
    }

    void Server::SendMessageToAllClients(std::vector<uint8_t>* packet) {
        m_networkManager->SendToAllClients(packet);
    }
}