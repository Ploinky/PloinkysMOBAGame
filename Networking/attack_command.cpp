#include "new_packets.h"

namespace PMG::Networking {
	void AttackCommandPacket::Read(std::vector<uint8_t>* data) {
		std::memcpy(&target_unit, data->data() + sizeof(packet_header_t), sizeof(target_unit));
	}

	void AttackCommandPacket::Write(std::vector<uint8_t>* data) {
		packet_header_t header{};
		header.type = type;
		header.size = sizeof(packet_header_t) + sizeof(target_unit);
		data->resize(header.size);
		std::memcpy(data->data(), &header, sizeof(packet_header_t));
		std::memcpy(data->data() + sizeof(packet_header_t), &target_unit, sizeof(target_unit));
	}
}