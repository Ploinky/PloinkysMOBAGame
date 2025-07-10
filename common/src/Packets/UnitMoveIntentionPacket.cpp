
#include <packets/UnitMoveIntentionPacket.h>
#include <cstring>

void UnitMoveIntentionPacket::Read(std::vector<uint8_t>* data) {
    int offset = sizeof(packet_header_t);

    memcpy(&unit, data->data() + offset, sizeof(unit));
    offset += sizeof(unit);

    memcpy(&x, data->data() + offset, sizeof(x));
    offset += sizeof(x);
    
    memcpy(&y, data->data() + offset, sizeof(y));
    offset += sizeof(y);
    
    memcpy(&z, data->data() + offset, sizeof(z));
    offset += sizeof(z);

}

void UnitMoveIntentionPacket::Write(std::vector<uint8_t>* data) {
    packet_header_t header{};
    header.type = type;
    header.size = sizeof(packet_header_t) + sizeof(unit) + sizeof(x) + sizeof(y) + sizeof(z);

    size_t offset = data->size();
    data->resize(data->size() + header.size);

    memcpy(data->data() + offset, &header, sizeof(header));
    offset += sizeof(header);

    memcpy(data->data() + offset, &unit, sizeof(unit));
    offset += sizeof(unit);
    
    memcpy(data->data() + offset, &x, sizeof(x));
    offset += sizeof(x);
    
    memcpy(data->data() + offset, &y, sizeof(y));
    offset += sizeof(y);
    
    memcpy(data->data() + offset, &z, sizeof(z));
};