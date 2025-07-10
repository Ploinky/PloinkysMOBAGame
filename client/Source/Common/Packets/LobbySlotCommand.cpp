#include <Common/Packets/LobbySlotCommand.h>
#include <cstring>

void LobbySlotCmd::Read(std::vector<uint8_t>* data) {
	packet_header_t header;

	memcpy(&header, data->data(), sizeof(packet_header_t));
	memcpy(&slot, data->data() + sizeof(packet_header_t), sizeof(slot));
}

void LobbySlotCmd::Write(std::vector<uint8_t>* data) {
	packet_header_t header;
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(slot);

	size_t offset = data->size();
	data->resize(data->size() + header.size);

	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
	memcpy(data->data() + offset + sizeof(packet_header_t), &slot, sizeof(slot));
}