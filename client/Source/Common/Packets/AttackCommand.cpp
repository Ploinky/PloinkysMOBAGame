#include <Common/Packets/AttackCommand.h>
#include <stdint.h>
#include <cstring>

void AttackCommandPacket::Read(std::vector<uint8_t>* data) {
	memcpy(&target_unit, data->data() + sizeof(packet_header_t), sizeof(target_unit));
}

void AttackCommandPacket::Write(std::vector<uint8_t>* data) {
	packet_header_t header{};
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(target_unit);

	size_t offset = data->size();
	data->resize(data->size() + header.size);
	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
	memcpy(data->data() + offset + sizeof(packet_header_t), &target_unit, sizeof(target_unit));
}