#include "new_packets.h"

namespace PMG::Networking {
	void GameTickPacket::Read(std::vector<uint8_t>* data) {
		packet_header_t header{};
		std::memcpy(&header, data->data(), sizeof(packet_header_t));

		this->data.resize(header.size);

		std::memcpy(this->data.data(), &header, sizeof(packet_header_t));
		std::memcpy(this->data.data() + sizeof(packet_header_t), data->data(), header.size - sizeof(packet_header_t));
	}

	void GameTickPacket::Write(std::vector<uint8_t>* data) {
		packet_header_t header;
		header.type = type;
		header.size = sizeof(packet_header_t) + this->data.size();

		int offset = data->size();
		data->resize(data->size() + header.size);

		std::memcpy(data->data() + offset, &header, sizeof(packet_header_t));
		std::memcpy(data->data() + offset + sizeof(packet_header_t), this->data.data(), this->data.size());
	}
}