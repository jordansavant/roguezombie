#pragma once
#ifndef RZ_MISSIONCLIENT_H
#define RZ_MISSIONCLIENT_H

#include <vector>
#include <map>
#include "../../bitengine/Network.hpp"
#include "LogicalType.hpp"
#include "JournalEntry.hpp"

class MissionClient
{
public:

    MissionClient();

    unsigned int id;
    bool isComplete;
    LogicalType logicalType;
    std::map<unsigned int, MissionClient> childMissions;
    JournalEntry::Entry journalEntry;

    void handleSnapshot(bit::ServerPacket &packet);
};

#endif