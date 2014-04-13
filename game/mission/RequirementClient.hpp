#pragma once
#ifndef RZ_REQUIREMENTCLIENT_H
#define RZ_REQUIREMENTCLIENT_H

#include "../../bitengine/Network.hpp"

class RequirementClient
{
public:

    RequirementClient();
    
    bool isFullfilled;

    virtual void handleSnapshot(bit::ServerPacket &packet);
};

#endif