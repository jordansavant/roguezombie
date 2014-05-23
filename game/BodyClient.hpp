#pragma once
#ifndef RZ_BODYCLIENT_H
#define RZ_BODYCLIENT_H

#include "Body.hpp"
#include "../bitengine/Network.hpp"
#include "items/ItemClient.hpp"
#include "LevelClient.hpp"

class BodyClient
{
public:

    BodyClient();

    Body::Schema schema;
    ItemClient inventoryClient;
    unsigned int lastSnapshotId;
    LevelClient* level;

    virtual void reinitialize();

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif