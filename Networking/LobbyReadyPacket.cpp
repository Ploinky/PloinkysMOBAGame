#include "new_packets.h"

namespace PMG::Networking {
	void LobbyReadyPck::Read(std::vector<uint8_t>* data) {
		packet_header_t header;

		std::memcpy(&header, data->data(), sizeof(packet_header_t));
		std::memcpy(&steamId, data->data() + sizeof(packet_header_t), sizeof(steamId));
	}

	void LobbyReadyPck::Write(std::vector<uint8_t>* data) {
		packet_header_t header;
		header.type = type;
		header.size = sizeof(packet_header_t) + sizeof(steamId);

		int offset = data->size();
		data->resize(data->size() + header.size);

		std::memcpy(data->data() + offset, &header, sizeof(packet_header_t));
		std::memcpy(data->data() + offset + sizeof(packet_header_t), &steamId, sizeof(steamId));
	}
}