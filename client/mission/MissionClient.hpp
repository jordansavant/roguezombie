#pragma once
#ifndef RZ_MISSIONCLIENT_H
#define RZ_MISSIONCLIENT_H

#include "../../server/mission/Mission.hpp"
#include <vector>
#include <map>
#include "../../bitengine/Network.hpp"
#include "../../server/mission/LogicalType.hpp"
#include "../../server/mission/JournalEntry.hpp"

class MissionClient
{
public:

    MissionClient();

    Mission::Schema schema;
    std::map<unsigned int, MissionClient> childMissions;

    void handleSnapshot(bit::ServerPacket &packet);
};

#endif