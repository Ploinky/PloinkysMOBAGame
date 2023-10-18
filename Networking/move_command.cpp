#include "new_packets.h"

namespace PMG::Networking {
	void MoveCommandPacket::Read(std::vector<uint8_t>* data) {
		std::memcpy(&x, data->data() + sizeof(packet_header_t), sizeof(x));
		std::memcpy(&y, data->data() + sizeof(packet_header_t) + sizeof(x), sizeof(y));
	}

	void MoveCommandPacket::Write(std::vector<uint8_t>* data) {
		packet_header_t header{};
		header.type = type;
		header.size = sizeof(packet_header_t) + sizeof(x) + sizeof(y);

		int offset = data->size();
		data->resize(data->size() + header.size);
		std::memcpy(data->data() + offset, &header, sizeof(packet_header_t));
		std::memcpy(data->data() + offset + sizeof(packet_header_t), &x, sizeof(x));
		std::memcpy(data->data() + offset + sizeof(packet_header_t) + sizeof(x), &y, sizeof(y));
	}
}