#include "new_packets.h"

namespace PMG::Networking {
	void DespawnPacket::Read(std::vector<uint8_t>* data) {
		int offset = sizeof(packet_header_t);

		std::memcpy(&unit, data->data() + offset, sizeof(unit));
	}

	void DespawnPacket::Write(std::vector<uint8_t>* data) {
		packet_header_t header{};
		header.type = type;
		header.size = sizeof(header) + sizeof(unit);

		data->resize(header.size);

		int offset = 0;
		std::memcpy(data->data() + offset, &header, sizeof(header));
		offset += sizeof(header);

		std::memcpy(data->data() + offset, &unit, sizeof(unit));
	}
}