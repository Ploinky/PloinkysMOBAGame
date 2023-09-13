#include "new_packets.h"

namespace PMG::Networking {
	void UnitIdPacket::Read(std::vector<uint8_t>* data) {
		int mem_index = sizeof(type) + sizeof(size_t); // skip type bytes & size bytes?! yikes
		std::memcpy(&unit_id, data->data(), sizeof(unit_id));
	}

	void UnitIdPacket::Write(std::vector<uint8_t>* data) {
		packet_header_t header;
		header.type = type;
		header.size = sizeof(packet_header_t) + sizeof(unit_id);

		data->resize(header.size);

		int mem_index = 0;
		std::memcpy(data->data(), &header.type, sizeof(packet_header_t));
		std::memcpy(data->data() + sizeof(packet_header_t), &unit_id, sizeof(unit_id));
	}
}