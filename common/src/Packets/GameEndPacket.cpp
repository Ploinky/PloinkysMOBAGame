#include <packets/GameEndPacket.h>
#include <cstring>

void GameEndPacket::Read(std::vector<uint8_t>* data) {
	memcpy(&victory, data->data() + sizeof(packet_header_t), sizeof(victory));
}

void GameEndPacket::Write(std::vector<uint8_t>* data) {
	packet_header_t header;
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(victory);
	data->resize(header.size);

	memcpy(data->data(), &header, sizeof(packet_header_t));
	memcpy(data->data() + sizeof(packet_header_t), &victory, sizeof(victory));
}