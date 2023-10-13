#include "Lobby.h"
#include "pmg_networking.h"
#include "Renderer.h"

namespace PMG {
	Lobby::Lobby(std::string server, IClientStateHandler* handler, int width, int height) : IClientState(handler, width, height) {
		networkManager_ = ServerNetworkManager();

        packetManager_ = Networking::NetworkHandlerManager<Networking::PacketType>();
        // Register network packets, the fuck...
        packetManager_.RegisterHandler(Networking::PacketType::LOBBY_PCK_SLOT, [this](std::vector<uint8_t> data) { HandleSlotPacket(data); });

        networkManager_.Initialize(&packetManager_);

        networkManager_.ConnectToServer(server);

        mySlot_ = -1;
	}

    void Lobby::Update(float dt) {
        networkManager_.ReceivePacket();
    }

    void Lobby::Render(Renderer* renderer) {
        int slotWidth = (windowWidth_ - 150) / 2;
        int slotHeight = (windowHeight_ - 440) / 5;

        for (int i = 0; i < 10; i++) {
            bool right = (i % 2) != 0;
            
            int ySlot = (i / 2);

            float color[3]{ 0.6, 0.6, 0.6 };
            if (players_[i] != nullptr) {
                color[0] = 1;
                color[1] = 0;
                color[2] = 0;
            }

            int x = 50 + (right ? slotWidth + 50 : 0);
            int y = 50 + ySlot * slotHeight + ySlot * 10;

            renderer->FillRect(x, y, slotWidth, slotHeight, color);

            if (players_[i] == nullptr) {
                renderer->RenderText(x, y, slotWidth, slotHeight, "Take slot");
            }
            else {
                renderer->RenderText(x, y, slotWidth, slotHeight, players_[i]->name);
            }
        }
    }

    void Lobby::HandleSlotPacket(std::vector<uint8_t> data) {
        Networking::LobbySlotPacket pck = Networking::LobbySlotPacket();
        pck.Read(&data);

        if (pck.steamId == SteamUser()->GetSteamID().ConvertToUint64()) {
            mySlot_ = pck.slot;
        }

        for (int i = 0; i < 10; i++) {
            if (players_[i] != nullptr && players_[i]->steamId == pck.steamId) {
                players_[pck.slot] = players_[i];
                players_[i] = nullptr;
                return;
            }
        }

        // must be a new player
        CSteamID id = CSteamID(pck.steamId);
        Player* p = new Player();
        p->name = std::string(SteamFriends()->GetFriendPersonaName(id));
        p->steamId = pck.steamId;
        players_[pck.slot] = p;
    }

    void Lobby::MouseButtonPressed(int button) {
        if (button != 0) {
            return;
        }

        int slotWidth = (windowWidth_ - 150) / 2;
        int slotHeight = (windowHeight_ - 440) / 5;

        for (int i = 0; i < 10; i++) {
            bool right = (i % 2) != 0;
            int ySlot = (i / 2);

            int x = 50 + (right ? slotWidth + 50 : 0);
            int y = 50 + ySlot * slotHeight + ySlot * 10;
            if (mouseX_ > x && mouseX_ < x + slotWidth
                && mouseY_ > y && mouseY_ < y + slotHeight) {
                Networking::LobbySlotPacket pck = Networking::LobbySlotPacket();
                pck.slot = i;

                networkManager_.SendPacket(&pck);
            }
        }

    }
}