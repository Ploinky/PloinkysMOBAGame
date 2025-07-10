#include <Common/Packets/LobbySlotPacket.h>
#include <cstring>

void LobbySlotPacket::Read(std::vector<uint8_t>* data) {
	packet_header_t header;

	memcpy(&header, data->data(), sizeof(packet_header_t));
	memcpy(&slot, data->data() + sizeof(packet_header_t), sizeof(slot));
	memcpy(&steamId, data->data() + sizeof(packet_header_t) + sizeof(slot), sizeof(steamId));
	memcpy(&isReady, data->data() + sizeof(packet_header_t) + sizeof(slot) + sizeof(steamId), sizeof(isReady));
}

void LobbySlotPacket::Write(std::vector<uint8_t>* data) {
	packet_header_t header;
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(slot) + sizeof(steamId) + sizeof(isReady);

	size_t offset = data->size();
	data->resize(data->size() + header.size);

	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
	memcpy(data->data() + offset + sizeof(packet_header_t), &slot, sizeof(slot));
	memcpy(data->data() + offset + sizeof(packet_header_t) + sizeof(slot), &steamId, sizeof(steamId));
	memcpy(data->data() + offset + sizeof(packet_header_t) + sizeof(slot) + sizeof(steamId), &isReady, sizeof(isReady));
}