#pragma once
#ifndef RZ_MISSIONCLIENT_H
#define RZ_MISSIONCLIENT_H

#include "Mission.hpp"
#include <vector>
#include <map>
#include "../../bitengine/Network.hpp"
#include "LogicalType.hpp"
#include "JournalEntry.hpp"

class MissionClient
{
public:

    MissionClient();

    Mission::Schema schema;
    std::map<unsigned int, MissionClient> childMissions;

    void handleSnapshot(bit::ServerPacket &packet);
};

#endif