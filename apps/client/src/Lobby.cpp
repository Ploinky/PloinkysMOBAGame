#include "Lobby.h"
#include "common/pmg_networking.h"
#include "Renderer.h"

Lobby::Lobby(std::string server, IClientStateHandler* handler, int width, int height) : IClientState(handler, width, height) {
	networkManager_ = new ClientNetworkManager();
	HBitmap hButton = handler->GetAssetManager()->GetBitmapImage("data/ui/buttons/menu-button/menu-button.bmp");

    packetManager_ = NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>>();
    // Register network packets, the fuck...
    packetManager_.RegisterHandler(PacketType::LOBBY_PLAYER_PCK, [this](std::vector<uint8_t> data) { HandleSlotPacket(data); });
    packetManager_.RegisterHandler(PacketType::CHARACER_SELECT_REMAINING_TIME, [this](std::vector<uint8_t> data) { HandleCharacterSelectTimePacket(data); });
    packetManager_.RegisterHandler(PacketType::LOBBY_GAME_START, [this](std::vector<uint8_t> data) { HandleGameStartPacket(data); });
    networkManager_->Initialize(&packetManager_);

    networkManager_->ConnectToServer(server);

    mySlot_ = -1;

    rootElement_ = GuiElement();
    rootElement_.m_pos = { 0, 0 };
    rootElement_.m_size = { static_cast<float>(windowWidth_), static_cast<float>(windowHeight_) };

    GuiButton* btnReady = new GuiButton();
    btnReady->m_pos = { 100.0f, windowHeight_ - 150.0f };
    btnReady->m_size = { 300, 80 };
    btnReady->m_text = "Ready";
    btnReady->m_color[0] = 0.2f;
    btnReady->m_color[1] = 0.6f;
    btnReady->m_color[2] = 0.2f;
    btnReady->hImage = hButton;
    btnReady->e_onMousePressed = [this]() {
        LobbyReadyCmd cmd;
        networkManager_->SendPacket(&cmd);
    };
    rootElement_.m_children.push_back(btnReady);

    GuiButton* btnBack = new GuiButton();
    btnBack->m_pos = { windowWidth_ - 400.0f, windowHeight_ - 150.0f };
    btnBack->m_size = { 300, 80 };
    btnBack->m_text = "Leave";
    btnBack->m_color[0] = 0.6f;
    btnBack->m_color[1] = 0.2f;
    btnBack->m_color[2] = 0.2f;
    btnBack->hImage = hButton;
    btnBack->e_onMousePressed = [this]() {
        handler_->OpenMainMenu();
    };

    rootElement_.m_children.push_back(btnBack);
}

Lobby::~Lobby() {
    //
    // if (networkManager_->IsConnected()) {
    //    networkManager_->Close();
    // }
}

void Lobby::Update(float dt) {
    networkManager_->ReceivePacket();
}

void Lobby::Render(CRenderer* renderer) {
    int slotWidth = (windowWidth_ - 150) / 2;
    int slotHeight = (windowHeight_ - 440) / 5;

    for (int i = 0; i < 10; i++) {
        bool right = (i % 2) != 0;
            
        int ySlot = (i / 2);

        float color[3]{ 0.6, 0.6, 0.6 };
        if (players_[i] != nullptr) {
            if (players_[i]->ready) {
                color[0] = 0;
                color[1] = 1;
                color[2] = 0;
            }
            else {
                color[0] = 1;
                color[1] = 0;
                color[2] = 0;
            }
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

    rootElement_.Render(renderer);
}

void Lobby::HandleSlotPacket(std::vector<uint8_t> data) {
    LobbySlotPacket pck = LobbySlotPacket();
    pck.Read(&data);

    // TODO
    if (pck.steamId == 0) {
        mySlot_ = pck.slot;
    }

    for (int i = 0; i < 10; i++) {
        if (players_[i] != nullptr && players_[i]->steamId == pck.steamId) {
            players_[pck.slot] = players_[i];
            players_[pck.slot]->ready = pck.isReady;

            if (pck.slot != i) {
                players_[i] = nullptr;
            }
            return;
        }
    }

    // must be a new player
    PlayerID id = pck.steamId;
    Player* p = new Player();
    p->name = "A Player";
    p->steamId = pck.steamId;
    p->ready = pck.isReady;
    players_[pck.slot] = p;
}

void Lobby::HandleGameStartPacket(std::vector<uint8_t> data) {
    LobbyGameStartPck pck = LobbyGameStartPck();
    pck.Read(&data);

    handler_->JoinGame(networkManager_);
}

void Lobby::HandleCharacterSelectTimePacket(std::vector<uint8_t> data) {
    CRemainingTimePacket pck = CRemainingTimePacket();
    pck.Read(&data);

    handler_->StartCharacterSelect(networkManager_, players_);
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
            LobbySlotCmd pck = LobbySlotCmd();
            pck.slot = i;

            networkManager_->SendPacket(&pck);
        }
    }

    rootElement_.MousePressed(mouseX_, mouseY_);
}

void Lobby::Action(EInputAction eAction) {
    switch(eAction) {
        case EInputAction::MENU_SELECT:
            rootElement_.MousePressed(mouseX_, mouseY_);
            break;
        case EInputAction::MENU_CLOSE:
            handler_->OpenMainMenu();
            break;
        default:
            break;
    }
}