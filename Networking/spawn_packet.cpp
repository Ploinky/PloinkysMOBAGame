#include "new_packets.h"

namespace PMG::Networking {
	void SpawnPacket::Read(std::vector<uint8_t>* data) {
		int offset = sizeof(packet_header_t);

		std::memcpy(&unit, data->data() + offset, sizeof(unit));
		offset += sizeof(unit);

		std::memcpy(&unit_type, data->data() + offset, sizeof(unit_type));
		offset += sizeof(unit_type);

		std::memcpy(&team, data->data() + offset, sizeof(team));
		offset += sizeof(team);

		std::memcpy(&x, data->data() + offset, sizeof(x));
		offset += sizeof(x);

		std::memcpy(&y, data->data() + offset, sizeof(y));
		offset += sizeof(y);

		std::memcpy(&z, data->data() + offset, sizeof(z));
	}

	void SpawnPacket::Write(std::vector<uint8_t>* data) {
		packet_header_t header{};
		header.type = type;
		header.size = sizeof(packet_header_t) + sizeof(unit) + sizeof(unit_type) + sizeof(team) + sizeof(x) + sizeof(y) + sizeof(z);

		int offset = data->size();
		data->resize(data->size() + header.size);


		std::memcpy(data->data() + offset, &header, sizeof(header));
		offset += sizeof(header);
		
		std::memcpy(data->data() + offset, &unit, sizeof(unit));
		offset += sizeof(unit);
		
		std::memcpy(data->data() + offset, &unit_type, sizeof(unit_type));
		offset += sizeof(unit_type);
		
		std::memcpy(data->data() + offset, &team, sizeof(team));
		offset += sizeof(team);
		
		std::memcpy(data->data() + offset, &x, sizeof(x));
		offset += sizeof(x);
		
		std::memcpy(data->data() + offset, &y, sizeof(y));
		offset += sizeof(y);

		std::memcpy(data->data() + offset, &z, sizeof(z));
	}
}