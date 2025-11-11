#include <packets/AttackStartPacket.h>
#include <cstring>

void AttackStartPacket::Read(std::vector<uint8_t>* data) {
	int offset = sizeof(packet_header_t);
	memcpy(&content, data->data() + offset, sizeof(content));
}

void AttackStartPacket::Write(std::vector<uint8_t>* data) {
	packet_header_t header{};
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(content);

	size_t offset = data->size();
	data->resize(data->size() + header.size);

	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
	offset += sizeof(packet_header_t);
	memcpy(data->data() + offset, &content, sizeof(content));
}