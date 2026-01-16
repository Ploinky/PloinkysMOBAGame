#include "packets/UnitDeathPacket.h"
#include <string.h>

void CUnitDeathPacket::Read(std::vector<uint8_t>* data) {
	packet_header_t header;

	memcpy(&header, data->data(), sizeof(packet_header_t));
	memcpy(&idUnit, data->data() + sizeof(packet_header_t), sizeof(idUnit));
}

void CUnitDeathPacket::Write(std::vector<uint8_t>* data) const {
	packet_header_t header;
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(idUnit);

	size_t offset = data->size();
	data->resize(data->size() + header.size);

	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
	memcpy(data->data() + offset + sizeof(packet_header_t), &idUnit, sizeof(idUnit));
}