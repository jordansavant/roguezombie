#include "BodyClient.hpp"

BodyClient::BodyClient()
    : schema(), lastSnapshotId(0), inventoryClient(), level(NULL)
{
}

void BodyClient::clientLoad(LevelClient* _level)
{
    level = _level;
}

void BodyClient::clientUpdate(sf::Time &gameTime)
{
}

void BodyClient::reinitialize()
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