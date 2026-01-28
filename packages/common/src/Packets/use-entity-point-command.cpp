#include <packets/use-entity-point-command.h>
#include <cstring>

void CUseEntityPointCommand::Read(std::vector<uint8_t>* data) {
	int offset = sizeof(packet_header_t);

	memcpy(&idEntity, data->data() + offset, sizeof(idEntity));
	offset += sizeof(idEntity);

	memcpy(&x, data->data() + offset, sizeof(x));
	offset += sizeof(x);

	memcpy(&y, data->data() + offset, sizeof(y));
}

void CUseEntityPointCommand::Write(std::vector<uint8_t>* data) const {
	packet_header_t header{};
	header.type = type;
	header.size = sizeof(packet_header_t) + sizeof(idEntity) + sizeof(x) + sizeof(y);

	size_t offset = data->size();
	data->resize(data->size() + header.size);


	memcpy(data->data() + offset, &header, sizeof(header));
	offset += sizeof(header);

	memcpy(data->data() + offset, &idEntity, sizeof(idEntity));
	offset += sizeof(idEntity);

	memcpy(data->data() + offset, &x, sizeof(x));
	offset += sizeof(x);

	memcpy(data->data() + offset, &y, sizeof(y));
	offset += sizeof(y);
}