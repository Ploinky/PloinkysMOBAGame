#include <Packets/LobbyPlayerLeftPacket.h>
#include <cstring>

void LobbyPlayerLeftPacket::Read(std::vector<uint8_t>* data) {
	packet_header_t header;

	memcpy(&header, data->data(), sizeof(packet_header_t));
	memcpy(&uPlayerId, data->data() + sizeof(packet_header_t), sizeof(uPlayerId));
}

void LobbyPlayerLeftPacket::Write(std::vector<uint8_t>* data) {
	packet_header_t header;
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(uPlayerId);

	size_t offset = data->size();
	data->resize(data->size() + header.size);

	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
	memcpy(data->data() + offset + sizeof(packet_header_t), &uPlayerId, sizeof(uPlayerId));
}