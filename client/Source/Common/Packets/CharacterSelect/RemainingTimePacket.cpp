#include <Common/Packets/CharacterSelect/RemainingTimePacket.h>

void CRemainingTimePacket::Read(std::vector<uint8_t>* data) {
	int offset = sizeof(packet_header_t);
	memcpy(&TimeInMS, data->data() + offset, sizeof(TimeInMS));
}

void CRemainingTimePacket::Write(std::vector<uint8_t>* data) {
	packet_header_t header{};
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(TimeInMS);

	size_t offset = data->size();
	data->resize(data->size() + header.size);

	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
	offset += sizeof(packet_header_t);
	memcpy(data->data() + offset, &TimeInMS, sizeof(TimeInMS));
}