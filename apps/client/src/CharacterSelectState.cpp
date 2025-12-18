#include "CharacterSelectState.h"

CCharacterSelectState::CCharacterSelectState(ClientNetworkManager* server, IClientStateHandler* handler, int width, int height, Player** players) : IClientState(handler, width, height) {
    networkManager_ = server;

    packetManager_ = NetworkHandlerManager<PacketType, std::function<void(std::vector<uint8_t>)>>();
    // Register network packets, the fuck...
    packetManager_.RegisterHandler(PacketType::CHARACER_SELECT_REMAINING_TIME, [this](std::vector<uint8_t> data) { HandleCharacterSelectTimePacket(data); });
    networkManager_->Initialize(handler->GetNetworkEngine(), &packetManager_);

    mySlot_ = -1;

    rootElement_ = GuiElement();
    rootElement_.m_pos = { 0, 0 };
    rootElement_.m_size = { static_cast<float>(windowWidth_), static_cast<float>(windowHeight_) };

    GuiButton* btnReady = new GuiButton();
    btnReady->m_pos = { windowWidth_ / 2.0f  - 150.f, windowHeight_ - 150.0f };
    btnReady->m_size = { 300, 80 };
    btnReady->m_text = "Lock In";
    btnReady->m_color[0] = 0.2f;
    btnReady->m_color[1] = 0.6f;
    btnReady->m_color[2] = 0.2f;
    btnReady->e_onMousePressed = [this]() {
        LobbyReadyCmd cmd;
        networkManager_->SendPacket(&cmd);
    };
    rootElement_.m_children.push_back(btnReady);
}

CCharacterSelectState::~CCharacterSelectState() {
    //
    // if (networkManager_->IsConnected()) {
    //    networkManager_->Close();
    // }
}

void CCharacterSelectState::Update(float dt) {
    m_iRemainingTime -= dt;

    networkManager_->ReceivePacket();
}

void CCharacterSelectState::Render(CRenderer* renderer) {
    rootElement_.Render(renderer);

    int iTimeReaminingSeconds = m_iRemainingTime / 1000;
    renderer->RenderText(windowWidth_ / 2 - 100, 50, 200, 20, std::to_string(iTimeReaminingSeconds).append(" s"));

    // renderer->DrawImage(windowWidth_ / 2 - 75, 100, 50, 50, "FootballPersonIcon");
    renderer->RenderText(windowWidth_ / 2 - 100, 150, 100, 100, "FootballPerson");
}

void CCharacterSelectState::HandleCharacterSelectTimePacket(std::vector<uint8_t> data) {
    // TODO display remaining time
}

void CCharacterSelectState::MouseButtonPressed(int button) {
    if (button != 0) {
        return;
    }

    rootElement_.MousePressed(mouseX_, mouseY_);
}
