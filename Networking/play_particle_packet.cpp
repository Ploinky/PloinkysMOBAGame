#include "new_packets.h"

namespace PMG::Networking {
	void PlayParticlePacket::Read(std::vector<uint8_t>* data) {
		packet_header_t header;

		std::memcpy(&header, data->data(), sizeof(packet_header_t));
		std::memcpy(&unit, data->data() + sizeof(packet_header_t), sizeof(unit));

		particle.resize(header.size - sizeof(unit) - sizeof(packet_header_t));
		std::memcpy(particle.data(), data->data() + sizeof(packet_header_t) + sizeof(unit), header.size - sizeof(unit) - sizeof(packet_header_t));
	}

	void PlayParticlePacket::Write(std::vector<uint8_t>* data) {
		packet_header_t header;
		header.type = type;
		header.size = sizeof(packet_header_t) + sizeof(unit) + particle.length();

		int offset = data->size();
		data->resize(data->size() + header.size);

		std::memcpy(data->data() + offset, &header, sizeof(packet_header_t));
		std::memcpy(data->data() + offset + sizeof(packet_header_t), &unit, sizeof(unit));
		std::memcpy(data->data() + offset + sizeof(packet_header_t) + sizeof(unit), particle.data(), particle.length());
	}
}