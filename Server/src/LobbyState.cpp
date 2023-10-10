#include "LobbyState.h"
#include "new_packets.h"
#include <vector>
namespace PMG {
	LobbyState::LobbyState() {
		networkManager_ = ServerNetworkManager();
		networkManager_.Initialize();
		networkManager_.CreateListenSocket(std::to_string(DEFAULT_PORT));

		networkManager_.on_clientConnected = [this](unsigned long newSocket) {
			for (int i = 0; i < 10; i++) {
				if (lobbySlots_[i] == nullptr) {
					LobbyPlayer* newPlayer = new LobbyPlayer();
					newPlayer->name = "";
					newPlayer->socket = newSocket;

					lobbySlots_[i] = newPlayer;

					Networking::LobbySlotPacket pck = Networking::LobbySlotPacket();
					pck.slot = i;

					networkManager_.SendToClient((HSteamNetConnection)newSocket, &pck);
					printf("Player connected, now %d players\n", GetPlayerCount());
					return;
				}
			}

			printf("Player could not be connected, now %d players\n", GetPlayerCount());
		};

		networkManager_.on_clientDisconnected = [this](unsigned long oldSocket) {
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
			Networking::LobbySlotCmd cmd = Networking::LobbySlotCmd();
			cmd.Read(data);
			
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

					networkManager_.SendToClient(conn, &pck);
				}
			}
		};
	}
	void LobbyState::Update(float dt) {
		networkManager_.Update();
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