#include "packets/round-reset-packet.h"
#include <string.h>

void CRoundResetPacket::Read(std::vector<uint8_t>* data) {
	packet_header_t header;

	memcpy(&header, data->data(), sizeof(packet_header_t));
}

void CRoundResetPacket::Write(std::vector<uint8_t>* data) const {
	packet_header_t header;
	header.type = type;
	header.size = sizeof(packet_header_t);

	size_t offset = data->size();
	data->resize(data->size() + header.size);

	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
}