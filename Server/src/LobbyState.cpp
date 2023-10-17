#include "LobbyState.h"
#include "new_packets.h"
#include <vector>
namespace PMG {
	LobbyState::LobbyState() {
		networkManager_ = ServerNetworkManager();
		networkManager_.Initialize();
		networkManager_.CreateListenSocket(std::to_string(DEFAULT_PORT));

		handler_.RegisterHandler(Networking::PacketType::LOBBY_PCK_SLOT, [this](std::vector<uint8_t> data, HSteamNetConnection conn) {
			Networking::LobbySlotCmd cmd = Networking::LobbySlotCmd();
			cmd.Read(&data);

			if (lobbySlots_[cmd.slot] != nullptr) {
				// TODO let client know the slot is taken
				return;
			}

			for (int i = 0; i < 10; i++) {
				if (lobbySlots_[i] != nullptr && lobbySlots_[i]->socket == conn) {
					if (cmd.slot == i) {
						// TODO client is already in requested slot?
						return;
					}
					lobbySlots_[cmd.slot] = lobbySlots_[i];
					lobbySlots_[i] = nullptr;

					Networking::LobbySlotPacket pck = Networking::LobbySlotPacket();
					pck.slot = cmd.slot;
					pck.steamId = lobbySlots_[cmd.slot]->steamId.ConvertToUint64();

					networkManager_.SendToClient(conn, &pck);
				}
			}
		});

		handler_.RegisterHandler(Networking::PacketType::LOBBY_CMD_READY, [this](std::vector<uint8_t> data, HSteamNetConnection conn) {
			Networking::LobbyReadyCmd cmd = Networking::LobbyReadyCmd();
			cmd.Read(&data);

			for (int i = 0; i < 10; i++) {
				if (lobbySlots_[i] != nullptr && lobbySlots_[i]->socket == conn) {
					lobbySlots_[i]->ready = !lobbySlots_[i]->ready;

					Networking::LobbyReadyPck pck = Networking::LobbyReadyPck();
					pck.steamId = lobbySlots_[i]->steamId.ConvertToUint64();

					std::vector<uint8_t> data;
					pck.Write(&data);
					networkManager_.SendToAllClients(&data);
					return;
				}
			}

			printf("Got lobby ready packet from unknown client");
		});

		networkManager_.on_clientConnected = [this](LobbyPlayer* newPlayer) {
			newPlayer->ready = false;

			for (int i = 0; i < 10; i++) {
				if (lobbySlots_[i] == nullptr) {
					lobbySlots_[i] = newPlayer;

					Networking::LobbySlotPacket pck = Networking::LobbySlotPacket();
					pck.slot = i;
					pck.steamId = newPlayer->steamId.ConvertToUint64();

					std::vector<uint8_t> data;
					pck.Write(&data);
					networkManager_.SendToAllClients(&data);
					printf("Player connected, now %d players\n", GetPlayerCount());
					return;
				}
			}

			printf("Player could not be connected, now %d players\n", GetPlayerCount());
		};

		networkManager_.on_clientDisconnected = [this](HSteamNetConnection oldSocket) {
			for (int i = 0; i < 10; i++) {
				if (lobbySlots_[i] != nullptr && lobbySlots_[i]->socket == oldSocket) {
					delete lobbySlots_[i];
					lobbySlots_[i] = nullptr;

					printf("Player disconnected, now %d players\n", GetPlayerCount());
					break;
				}
			}

			printf("Player could not be disconnected, now %d players\n", GetPlayerCount());
		};

		networkManager_.on_clientMessageReceived = [this](HSteamNetConnection conn, std::vector<uint8_t>* data) {
			Networking::packet_header_t header;
			std::memcpy(&header, data->data(), sizeof(Networking::packet_header_t));

			std::function fun = handler_.GetHandler(header.type);

			if (fun == nullptr) {
				printf("Missing handler for packet!");
				return;
			}
			fun(*data, conn);
		};
	}
	void LobbyState::Update(float dt) {
		networkManager_.Update();

		if (GetPlayerCount() > 0 && CheckAllReady()) {
			// start game!
			printf("Start!");
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
}