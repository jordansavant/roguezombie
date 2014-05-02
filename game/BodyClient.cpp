#include "BodyClient.hpp"

BodyClient::BodyClient()
    : schema(), inventoryClient()
{
}

void BodyClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    packet >> schema;

    // Item Clientside
    bool hasInventory;
    packet >> hasInventory;
    if(hasInventory)
    {
        inventoryClient.handleSnapshot(packet);
    }
}