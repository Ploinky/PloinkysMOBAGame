#include "new_packets.h"

namespace PMG::Networking {
	void LobbyReadyCmd::Read(std::vector<uint8_t>* data) {
		packet_header_t header;

		std::memcpy(&header, data->data(), sizeof(packet_header_t));
	}

	void LobbyReadyCmd::Write(std::vector<uint8_t>* data) {
		packet_header_t header;
		header.type = type;
		header.size = sizeof(packet_header_t);

		int offset = data->size();
		data->resize(data->size() + header.size);

		std::memcpy(data->data() + offset, &header, sizeof(packet_header_t));
	}
}