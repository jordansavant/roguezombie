#include "BodyClient.hpp"

BodyClient::BodyClient()
    : schema()
{
}

void BodyClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    packet >> schema;
}