#include <Packets/UnitStatsPacket.h>
#include <cstring>

namespace PMG::Networking {
	void UnitStatsPacket::Read(std::vector<uint8_t>* data) {
		int offset = sizeof(packet_header_t);

		memcpy(&unit, data->data() + offset, sizeof(unit));
		offset += sizeof(unit);

		memcpy(&health, data->data() + offset, sizeof(health));
		offset += sizeof(health);

		memcpy(&max_health, data->data() + offset, sizeof(max_health));
	}

	void UnitStatsPacket::Write(std::vector<uint8_t>* data) {
		packet_header_t header{};
		header.type = type;
		header.size = sizeof(packet_header_t) + sizeof(unit) + sizeof(health) + sizeof(max_health);

		size_t offset = data->size();
		data->resize(data->size() + header.size);


		memcpy(data->data() + offset, &header, sizeof(header));
		offset += sizeof(header);

		memcpy(data->data() + offset, &unit, sizeof(unit));
		offset += sizeof(unit);

		memcpy(data->data() + offset, &health, sizeof(health));
		offset += sizeof(health);

		memcpy(data->data() + offset, &max_health, sizeof(max_health));
		offset += sizeof(max_health);
	}
}