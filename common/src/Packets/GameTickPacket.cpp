#include <Packets/GameTickPacket.h>
#include <cstring>

void GameTickPacket::Read(std::vector<uint8_t>* data) {
	memcpy(&tick, data->data() + sizeof(packet_header_t), sizeof(tick));
}

void GameTickPacket::Write(std::vector<uint8_t>* data) {
	packet_header_t header;
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(tick);
	data->resize(header.size);

	memcpy(data->data(), &header, sizeof(packet_header_t));
	memcpy(data->data() + sizeof(packet_header_t), &tick, sizeof(tick));
}