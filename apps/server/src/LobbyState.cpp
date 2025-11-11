#include <LobbyState.h>
#include <common/PMG_Common.h>
#include <vector>

LobbyState::LobbyState(IServerStateHandler* handler) : IServerState(handler) {
	networkManager_ = new ServerNetworkManager();
	networkManager_->Initialize();
	Initialize();
}

LobbyState::LobbyState(IServerStateHandler* handler, ServerNetworkManager* networkManager) : networkManager_(networkManager), IServerState(handler) {
	Initialize();
}

void LobbyState::Initialize() {
	networkManager_->CreateListenSocket(std::to_string(DEFAULT_PORT));

	packetHandler_.RegisterHandler(PacketType::LOBBY_CMD_SLOT, [this](std::vector<uint8_t> data, PlayerID idPlayer) {
		LobbySlotCmd cmd = LobbySlotCmd();
		cmd.Read(&data);

		if (lobbySlots_[cmd.slot] != nullptr) {
			// TODO let client know the slot is taken
			return;
		}

		for (int i = 0; i < 10; i++) {
			if (lobbySlots_[i] != nullptr && lobbySlots_[i]->idPlayer == idPlayer) {
				if (cmd.slot == i) {
					// TODO client is already in requested slot?
					return;
				}
				lobbySlots_[cmd.slot] = lobbySlots_[i];
				lobbySlots_[i] = nullptr;

				lobbySlots_[cmd.slot]->slot = cmd.slot;

				LobbySlotPacket pck = LobbySlotPacket();
				pck.slot = cmd.slot;
				pck.steamId = lobbySlots_[cmd.slot]->idPlayer.ConvertToUint64();
				pck.isReady = lobbySlots_[cmd.slot]->ready;

				std::vector<uint8_t> data;
				pck.Write(&data);

				networkManager_->SendToAllClients(&data);
			}
		}
	});

	packetHandler_.RegisterHandler(PacketType::LOBBY_CMD_READY, [this](std::vector<uint8_t> data, PlayerID idPlayer) {
		LobbyReadyCmd cmd = LobbyReadyCmd();
		cmd.Read(&data);

		for (int i = 0; i < 10; i++) {
			if (lobbySlots_[i] != nullptr && lobbySlots_[i]->idPlayer == idPlayer) {
				lobbySlots_[i]->ready = !lobbySlots_[i]->ready;

				LobbySlotPacket pck = LobbySlotPacket();
				pck.slot = i;
				pck.steamId = lobbySlots_[i]->idPlayer.ConvertToUint64();
				pck.isReady = lobbySlots_[i]->ready;

				std::vector<uint8_t> data;
				pck.Write(&data);
				networkManager_->SendToAllClients(&data);
				return;
			}
		}

		printf("Got lobby ready packet from unknown client");
	});

	networkManager_->on_clientConnected = [this](PlayerID newidPlayer) {
		LobbyPlayer* newPlayer = new LobbyPlayer();
		newPlayer->ready = false;
		newPlayer->idPlayer = newidPlayer;

		bool hasSlot = false;

		for (int i = 0; i < 10; i++) {
			if (!hasSlot && lobbySlots_[i] == nullptr) {
				lobbySlots_[i] = newPlayer;
				newPlayer->slot = i;

				LobbySlotPacket pck = LobbySlotPacket();
				pck.slot = i;
				pck.steamId = newPlayer->idPlayer.ConvertToUint64();
				pck.isReady = newPlayer->ready;

				std::vector<uint8_t> data;
				pck.Write(&data);
				networkManager_->SendToAllClients(&data);

				printf("Player connected, now %d players\n", GetPlayerCount());

				hasSlot = true;
			}
			else if (lobbySlots_[i] != nullptr) {
				LobbySlotPacket pck = LobbySlotPacket();
				pck.slot = i;
				pck.steamId = lobbySlots_[i]->idPlayer.ConvertToUint64();
				pck.isReady = lobbySlots_[i]->ready;

				std::vector<uint8_t> data;
				pck.Write(&data);
				networkManager_->SendToClient(newidPlayer, &pck);
			}
		}

		// TODO what if we cannot connect the player?
	};

	networkManager_->on_clientDisconnected = [this](PlayerID idPlayer) {
		for (int i = 0; i < 10; i++) {
			if (lobbySlots_[i] != nullptr && lobbySlots_[i]->idPlayer == idPlayer) {
				delete lobbySlots_[i];
				lobbySlots_[i] = nullptr;

				printf("Player disconnected, now %d players\n", GetPlayerCount());
				break;
			}
		}

		printf("Player could not be disconnected, now %d players\n", GetPlayerCount());
	};

	networkManager_->on_clientMessageReceived = [this](PlayerID idPlayer, std::vector<uint8_t>* data) {
		packet_header_t header;
		memcpy(&header, data->data(), sizeof(packet_header_t));

		std::function fun = packetHandler_.GetHandler(header.type);

		if (fun == nullptr) {
			printf("Missing handler for packet!");
			return;
		}
		fun(*data, idPlayer);
	};
}
void LobbyState::Update(float dt) {
	networkManager_->Update();

	if (GetPlayerCount() > 0 && CheckAllReady()) {
		// TODO we do really not want to accept more connections
		// networkManager_->StopListenSocket();

		// start game!
		LobbyGameStartPck pck;
		
		std::vector<uint8_t> data;
		pck.Write(&data);

		networkManager_->SendToAllClients(&pck);

		handler_->StartGame(networkManager_, lobbySlots_);
	}
}

bool LobbyState::CheckAllReady() {
	for (int i = 0; i < 10; i++) {
		if (lobbySlots_[i] != nullptr && !lobbySlots_[i]->ready) {
			return false;
		}
	}

	return true;
}

int LobbyState::GetPlayerCount() {
	int count = 0;

	for (int i = 0; i < 10; i++) {
		if (lobbySlots_[i] != nullptr) {
			count++;
		}
	}

	return count;
}