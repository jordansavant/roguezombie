#pragma once
#ifndef RZ_ITEMCLIENT_H
#define RZ_ITEMCLIENT_H

#include "Item.hpp"
#include "../../bitengine/Network.hpp"
#include <map>

class ItemClient
{
public:

    ItemClient();

    virtual ~ItemClient();

    Item::Schema schema;
    std::map<unsigned int, ItemClient> itemClients;

    void handleSnapshot(bit::ServerPacket &packet);
};

#endif