#include <packets/PlayParticlePacket.h>
#include <cstring>

void PlayParticlePacket::Read(std::vector<uint8_t>* data) {
	packet_header_t header;

	memcpy(&header, data->data(), sizeof(packet_header_t));
	memcpy(&x, data->data() + sizeof(packet_header_t), sizeof(unit));
	memcpy(&y, data->data() + sizeof(packet_header_t), sizeof(unit) + sizeof(x));
	memcpy(&unit, data->data() + sizeof(packet_header_t), sizeof(unit) + sizeof(x) + sizeof(y));

	particle.resize(header.size - sizeof(unit) - sizeof(x) - sizeof(y) - sizeof(packet_header_t));
	memcpy(particle.data(), data->data() + sizeof(packet_header_t) + sizeof(unit) + sizeof(x) + sizeof(y), header.size - sizeof(unit) - sizeof(x) - sizeof(y) - sizeof(packet_header_t));
}

void PlayParticlePacket::Write(std::vector<uint8_t>* data) const {
	packet_header_t header;
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(unit) + sizeof(x) + sizeof(y) + particle.length();

	size_t offset = data->size();
	data->resize(data->size() + header.size);

	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
	memcpy(data->data() + offset + sizeof(packet_header_t), &unit, sizeof(unit));
	memcpy(data->data() + offset + sizeof(packet_header_t) + sizeof(unit), &x, sizeof(x));
	memcpy(data->data() + offset + sizeof(packet_header_t) + sizeof(unit) + sizeof(x), &y, sizeof(y));
	memcpy(data->data() + offset + sizeof(packet_header_t) + sizeof(unit) + sizeof(x) + sizeof(y), particle.data(), particle.length());
}