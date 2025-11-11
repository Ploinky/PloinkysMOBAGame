#include <packets/LobbyReadyCommand.h>
#include <cstring>

void LobbyReadyCmd::Read(std::vector<uint8_t>* data) {
	packet_header_t header;

	memcpy(&header, data->data(), sizeof(packet_header_t));
}

void LobbyReadyCmd::Write(std::vector<uint8_t>* data) {
	packet_header_t header;
	header.type = type;
	header.size = sizeof(packet_header_t);

	size_t offset = data->size();
	data->resize(data->size() + header.size);

	memcpy(data->data() + offset, &header, sizeof(packet_header_t));
}