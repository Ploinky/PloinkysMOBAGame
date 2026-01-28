#include <packets/picked-up-entity-packet.h>
#include <cstring>

void CPickedUpEntityPacket::Read(std::vector<uint8_t>* data) {
	memcpy(&idUnit, data->data() + sizeof(packet_header_t), sizeof(idUnit));
	memcpy(&idPickedUpUnit, data->data() + sizeof(packet_header_t) + sizeof(idUnit), sizeof(idPickedUpUnit));
}

void CPickedUpEntityPacket::Write(std::vector<uint8_t>* data) const {
	packet_header_t header{};
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(idUnit) + sizeof(idPickedUpUnit);

	size_t offset = data->size();
	data->resize(data->size() + header.size);
	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
	memcpy(data->data() + offset + sizeof(packet_header_t), &idUnit, sizeof(idUnit));
	memcpy(data->data() + offset + sizeof(packet_header_t) + sizeof(idUnit), &idPickedUpUnit, sizeof(idPickedUpUnit));
}