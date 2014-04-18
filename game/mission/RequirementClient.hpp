#pragma once
#ifndef RZ_REQUIREMENTCLIENT_H
#define RZ_REQUIREMENTCLIENT_H

#include "../../bitengine/Network.hpp"
#include "JournalEntry.hpp"

class RequirementClient
{
public:

    RequirementClient();
    
    unsigned int id;
    bool isFullfilled;
    JournalEntry journalEntry;

    virtual void handleSnapshot(bit::ServerPacket &packet);
};

#endif