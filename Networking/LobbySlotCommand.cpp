#include "new_packets.h"

namespace PMG::Networking {
	void LobbySlotCmd::Read(std::vector<uint8_t>* data) {
		packet_header_t header;

		std::memcpy(&header, data->data(), sizeof(packet_header_t));
		std::memcpy(&slot, data->data() + sizeof(packet_header_t), sizeof(slot));
	}

	void LobbySlotCmd::Write(std::vector<uint8_t>* data) {
		packet_header_t header;
		header.type = type;
		header.size = sizeof(packet_header_t) + sizeof(slot);

		int offset = data->size();
		data->resize(data->size() + header.size);

		std::memcpy(data->data() + offset, &header, sizeof(packet_header_t));
		std::memcpy(data->data() + offset + sizeof(packet_header_t), &slot, sizeof(slot));
	}
}