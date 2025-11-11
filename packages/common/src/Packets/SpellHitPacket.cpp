#include "packets/SpellHitPacket.h"
#include <cstring>

void SpellHitPacket::Read(std::vector<uint8_t>* data) {
	packet_header_t header;

	memcpy(&header, data->data(), sizeof(packet_header_t));
	memcpy(&unit, data->data() + sizeof(packet_header_t), sizeof(unit));
	spell.resize(header.size - sizeof(unit) - sizeof(packet_header_t));
	memcpy(spell.data(), data->data() + sizeof(packet_header_t) + sizeof(unit), header.size - sizeof(unit) - sizeof(packet_header_t));
}

void SpellHitPacket::Write(std::vector<uint8_t>* data) {
	packet_header_t header;
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(unit) + spell.length();

	size_t offset = data->size();
	data->resize(data->size() + header.size);

	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
	memcpy(data->data() + offset + sizeof(packet_header_t), &unit, sizeof(unit));
	memcpy(data->data() + offset + sizeof(packet_header_t) + sizeof(unit), spell.data(), spell.length());
}