#include "new_packets.h"

namespace PMG::Networking {
	void CastCommandPacket::Read(std::vector<uint8_t>* data) {
		int offset = sizeof(packet_header_t);
		std::memcpy(&spell_slot, data->data() + offset, sizeof(spell_slot));
		offset += sizeof(spell_slot);
		std::memcpy(&x, data->data() + offset, sizeof(x));
		offset += sizeof(x);
		std::memcpy(&y, data->data() + offset, sizeof(y));
		offset += sizeof(y);
		std::memcpy(&z, data->data() + offset, sizeof(z));
	}

	void CastCommandPacket::Write(std::vector<uint8_t>* data) {
		packet_header_t header{};
		header.type = type;
		header.size = sizeof(packet_header_t) + sizeof(spell_slot) + sizeof(x) + sizeof(y) + sizeof(z);

		data->resize(header.size);

		int offset = 0;
		std::memcpy(data->data() + offset, &header, sizeof(packet_header_t));
		offset += sizeof(packet_header_t);
		std::memcpy(data->data() + offset, &spell_slot, sizeof(spell_slot));
		offset += sizeof(spell_slot);
		std::memcpy(data->data() + offset, &x, sizeof(x));
		offset += sizeof(x);
		std::memcpy(data->data() + offset, &y, sizeof(y));
		offset += sizeof(y);
		std::memcpy(data->data() + offset, &z, sizeof(z));
	}
}