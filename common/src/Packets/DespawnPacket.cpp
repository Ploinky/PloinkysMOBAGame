#include <Packets/DespawnPacket.h>
#include <cstring>

void DespawnPacket::Read(std::vector<uint8_t>* data) {
	int offset = sizeof(packet_header_t);

	memcpy(&unit, data->data() + offset, sizeof(unit));
}

void DespawnPacket::Write(std::vector<uint8_t>* data) {
	packet_header_t header{};
	header.type = type;
	header.size = sizeof(header) + sizeof(unit);

	size_t offset = data->size();
	data->resize(data->size() + header.size);

	memcpy(data->data() + offset, &header, sizeof(header));
	offset += sizeof(header);

	memcpy(data->data() + offset, &unit, sizeof(unit));
}