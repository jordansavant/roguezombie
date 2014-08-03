#include "ItemClient.hpp"
#include "../../bitengine/Network.hpp"

ItemClient::ItemClient()
    : schema()
{
}

ItemClient::~ItemClient()
{
}

void ItemClient::handleSnapshot(bit::ServerPacket& packet)
{
    packet >> schema;

    unsigned int itemSize;
    packet >> itemSize;
    itemClients.clear();
    for(unsigned int i=0; i < itemSize; i++)
    {
        unsigned int itemId;
        packet >> itemId;
        itemClients[itemId] = ItemClient();
        itemClients[itemId].handleSnapshot(packet);
    }
}