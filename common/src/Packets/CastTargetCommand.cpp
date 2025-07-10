#include <Packets/CastTargetCommand.h>
#include <cstring>

void CastTargetCommandPacket::Read(std::vector<uint8_t>* data) {
	memcpy(&spell_slot, data->data() + sizeof(packet_header_t), sizeof(spell_slot));
	memcpy(&target, data->data() + sizeof(packet_header_t) + sizeof(spell_slot), sizeof(target));
}

void CastTargetCommandPacket::Write(std::vector<uint8_t>* data) {
	packet_header_t header{};
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(spell_slot) + sizeof(target);

	size_t offset = data->size();
	data->resize(data->size() + header.size);

	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
	memcpy(data->data() + offset + sizeof(packet_header_t), &spell_slot, sizeof(spell_slot));
	memcpy(data->data() + offset + sizeof(packet_header_t) + sizeof(spell_slot), &target, sizeof(target));
}
