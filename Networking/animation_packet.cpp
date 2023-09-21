#include "new_packets.h"

namespace PMG::Networking {
	void AnimationPacket::Read(std::vector<uint8_t>* data) {
		packet_header_t header;

		std::memcpy(&header, data->data(), sizeof(packet_header_t));
		std::memcpy(&unit_id, data->data() + sizeof(packet_header_t), sizeof(unit_id));
		std::memcpy(animation_name.data(), data->data() + sizeof(packet_header_t) + sizeof(unit_id), header.size - sizeof(unit_id) - sizeof(packet_header_t));
	}

	void AnimationPacket::Write(std::vector<uint8_t>* data) {
		packet_header_t header;
		header.type = type;
		header.size = sizeof(packet_header_t) + sizeof(unit_id) + animation_name.length();

		int offset = data->size();
		data->resize(data->size() + header.size);

		std::memcpy(data->data() + offset, &header, sizeof(packet_header_t));
		std::memcpy(data->data() + offset + sizeof(packet_header_t), &unit_id, sizeof(unit_id));
		std::memcpy(data->data() + offset + sizeof(packet_header_t) + sizeof(unit_id), animation_name.data(), animation_name.length());
	}
}