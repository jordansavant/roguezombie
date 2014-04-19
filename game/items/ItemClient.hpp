#pragma once
#ifndef RZ_ITEMCLIENT_H
#define RZ_ITEMCLIENT_H

#include "Item.hpp"
#include "../../bitengine/Network.hpp"
#include <map>

class ItemClient : public Item
{
public:

    ItemClient();

    virtual ~ItemClient();
    std::map<unsigned int, ItemClient> itemClients;

    void handleSnapshot(bit::ServerPacket &packet);
};

#endif