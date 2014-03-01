#include "ServerPacket.hpp"

bit::ServerPacket::ServerPacket()
    : sf::Packet()
{
}

const void* bit::ServerPacket::onSend(std::size_t& size)
{
    return sf::Packet::onSend(size);
}

void bit::ServerPacket::onReceive(const void* data, std::size_t size)
{
    return sf::Packet::onReceive(data, size);
}