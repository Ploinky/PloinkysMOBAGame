#include "new_packets.h"

namespace PMG::Networking {
	void UnitStatsPacket::Read(std::vector<uint8_t>* data) {
		int offset = sizeof(packet_header_t);

		std::memcpy(&unit, data->data() + offset, sizeof(unit));
		offset += sizeof(unit);

		std::memcpy(&health, data->data() + offset, sizeof(health));
		offset += sizeof(health);

		std::memcpy(&max_health, data->data() + offset, sizeof(max_health));
	}

	void UnitStatsPacket::Write(std::vector<uint8_t>* data) {
		packet_header_t header{};
		header.type = type;
		header.size = sizeof(packet_header_t) + sizeof(unit) + sizeof(health) + sizeof(max_health);

		int offset = data->size();
		data->resize(data->size() + header.size);


		std::memcpy(data->data() + offset, &header, sizeof(header));
		offset += sizeof(header);

		std::memcpy(data->data() + offset, &unit, sizeof(unit));
		offset += sizeof(unit);

		std::memcpy(data->data() + offset, &health, sizeof(health));
		offset += sizeof(health);

		std::memcpy(data->data() + offset, &max_health, sizeof(max_health));
		offset += sizeof(max_health);
	}
}