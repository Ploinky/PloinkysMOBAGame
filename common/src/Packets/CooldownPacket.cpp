#include <Packets/CooldownPacket.h>
#include <cstring>

/*
    UnitId unit;
    int spell_slot;
    int cooldown;
    int total_cooldown;
*/

void CooldownPacket::Read(std::vector<uint8_t>* data) {
    int offset = sizeof(packet_header_t);

    memcpy(&unit, data->data() + offset, sizeof(unit));
    offset += sizeof(unit);

    memcpy(&spell_slot, data->data() + offset, sizeof(spell_slot));
    offset += sizeof(spell_slot);

    memcpy(&cooldown, data->data() + offset, sizeof(cooldown));
    offset += sizeof(cooldown);

    memcpy(&total_cooldown, data->data() + offset, sizeof(total_cooldown));
}

void CooldownPacket::Write(std::vector<uint8_t>* data) {
    packet_header_t header{};
    header.type = type;
    header.size = sizeof(packet_header_t) + sizeof(unit) + sizeof(spell_slot) + sizeof(cooldown) + sizeof(total_cooldown);

    size_t offset = data->size();

    data->resize(data->size() + header.size);

    memcpy(data->data() + offset, &header, sizeof(packet_header_t));
    offset += sizeof(packet_header_t);

    memcpy(data->data() + offset, &unit, sizeof(unit));
    offset += sizeof(unit);

    memcpy(data->data() + offset, &spell_slot, sizeof(spell_slot));
    offset += sizeof(spell_slot);

    memcpy(data->data() + offset, &cooldown, sizeof(cooldown));
    offset += sizeof(cooldown);

    memcpy(data->data() + offset, &total_cooldown, sizeof(total_cooldown));
}