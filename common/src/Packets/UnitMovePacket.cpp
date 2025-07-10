#include <Packets/UnitMovePacket.h>
#include <cstring>

void UnitMovePacket::Read(std::vector<uint8_t>* data) {
    int offset = sizeof(packet_header_t);

    memcpy(&unit, data->data() + offset, sizeof(unit));
    offset += sizeof(unit);

    memcpy(&x, data->data() + offset, sizeof(x));
    offset += sizeof(x);
    
    memcpy(&y, data->data() + offset, sizeof(y));
    offset += sizeof(y);
    
    memcpy(&z, data->data() + offset, sizeof(z));
    offset += sizeof(z);
    
    memcpy(&r, data->data() + offset, sizeof(r));
    offset += sizeof(r);

}

void UnitMovePacket::Write(std::vector<uint8_t>* data) {
    packet_header_t header{};
    header.type = type;
    header.size = sizeof(packet_header_t) + sizeof(unit) + sizeof(x) + sizeof(y) + sizeof(z) + sizeof(r);

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
    offset += sizeof(z);
    
    memcpy(data->data() + offset, &r, sizeof(r));
};