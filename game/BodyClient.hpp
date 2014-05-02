#pragma once
#ifndef RZ_BODYCLIENT_H
#define RZ_BODYCLIENT_H

#include "Body.hpp"
#include "../bitengine/Network.hpp"
#include "items/ItemClient.hpp"

class BodyClient
{
public:

    BodyClient();

    Body::Schema schema;
    ItemClient inventoryClient;

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif