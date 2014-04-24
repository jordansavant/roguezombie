#pragma once
#ifndef RZ_BODYCLIENT_H
#define RZ_BODYCLIENT_H

#include "Body.hpp"
#include "../bitengine/Network.hpp"

class BodyClient
{
public:

    BodyClient();
    
    Body::Schema schema;

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif