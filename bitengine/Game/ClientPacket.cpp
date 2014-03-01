#include "ClientPacket.hpp"

bit::ClientPacket::ClientPacket()
    : sf::Packet()
{
}

const void* bit::ClientPacket::onSend(std::size_t& size)
{
    return sf::Packet::onSend(size);
}

void bit::ClientPacket::onReceive(const void* data, std::size_t size)
{
    return sf::Packet::onReceive(data, size);
}