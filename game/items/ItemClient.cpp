#include "ItemClient.hpp"
#include "../../bitengine/Network.hpp"

ItemClient::ItemClient()
    : Item()
{
}

ItemClient::~ItemClient()
{
}

void ItemClient::handleSnapshot(bit::ServerPacket& packet)
{
    packet >> CategoryBase;
    packet >> CategoryArmor;
    packet >> CategoryWeapon;
    packet >> CategoryJewelry;
    packet >> CategoryContainer;

    unsigned int type_int;
    packet >> type_int;
    type = static_cast<Item::Type>(type_int);
    packet >> weight;

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