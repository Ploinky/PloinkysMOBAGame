#include <Common/Packets/MoveCommand.h>
#include <cstring>

void MoveCommandPacket::Read(std::vector<uint8_t>* data) {
	memcpy(&x, data->data() + sizeof(packet_header_t), sizeof(x));
	memcpy(&y, data->data() + sizeof(packet_header_t) + sizeof(x), sizeof(y));
}

void MoveCommandPacket::Write(std::vector<uint8_t>* data) {
	packet_header_t header{};
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(x) + sizeof(y);

	size_t offset = data->size();
	data->resize(data->size() + header.size);
	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
	memcpy(data->data() + offset + sizeof(packet_header_t), &x, sizeof(x));
	memcpy(data->data() + offset + sizeof(packet_header_t) + sizeof(x), &y, sizeof(y));
}