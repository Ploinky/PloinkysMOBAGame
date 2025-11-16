#include <packets/CastCommand.h>
#include <cstring>

void CastCommandPacket::Read(std::vector<uint8_t>* data) {
	int offset = sizeof(packet_header_t);
	memcpy(&spell_slot, data->data() + offset, sizeof(spell_slot));
	offset += sizeof(spell_slot);
	memcpy(&x, data->data() + offset, sizeof(x));
	offset += sizeof(x);
	memcpy(&y, data->data() + offset, sizeof(y));
	offset += sizeof(y);
	memcpy(&z, data->data() + offset, sizeof(z));
}

void CastCommandPacket::Write(std::vector<uint8_t>* data) {
	packet_header_t header{};
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(spell_slot) + sizeof(x) + sizeof(y) + sizeof(z);

	size_t offset = data->size();
	data->resize(data->size() + header.size);

	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
	offset += sizeof(packet_header_t);
	memcpy(data->data() + offset, &spell_slot, sizeof(spell_slot));
	offset += sizeof(spell_slot);
	memcpy(data->data() + offset, &x, sizeof(x));
	offset += sizeof(x);
	memcpy(data->data() + offset, &y, sizeof(y));
	offset += sizeof(y);
	memcpy(data->data() + offset, &z, sizeof(z));
}