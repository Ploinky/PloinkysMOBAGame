#include <Packets/GameTickPacket.h>
#include <cstring>

namespace PMG::Networking {
	void GameTickPacket::Read(std::vector<uint8_t>* data) {
		packet_header_t header{};
		memcpy(&header, data->data(), sizeof(packet_header_t));

		this->data.resize(header.size);

		memcpy(this->data.data(), &header, sizeof(packet_header_t));
		memcpy(this->data.data() + sizeof(packet_header_t), data->data(), header.size - sizeof(packet_header_t));
	}

	void GameTickPacket::Write(std::vector<uint8_t>* data) {
		packet_header_t header;
		header.type = type;
		header.size = sizeof(packet_header_t) + this->data.size();

		size_t offset = data->size();
		data->resize(data->size() + header.size);

		memcpy(data->data() + offset, &header, sizeof(packet_header_t));
		memcpy(data->data() + offset + sizeof(packet_header_t), this->data.data(), this->data.size());
	}
}