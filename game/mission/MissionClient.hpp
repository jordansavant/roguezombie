#pragma once
#ifndef RZ_MISSIONCLIENT_H
#define RZ_MISSIONCLIENT_H

#include <vector>
#include "../../bitengine/Network.hpp"
#include "RequirementClient.hpp"
#include "LogicalType.hpp"

class MissionClient
{
public:

    MissionClient();

    bool isComplete;
    LogicalType logicalType;
    std::vector<MissionClient> childMissions;
    std::vector<RequirementClient> requirements;

    void handleSnapshot(bit::ServerPacket &packet);

};

#endif