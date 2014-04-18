#pragma once
#ifndef RZ_MISSIONCLIENT_H
#define RZ_MISSIONCLIENT_H

#include <vector>
#include <map>
#include "../../bitengine/Network.hpp"
#include "RequirementClient.hpp"
#include "LogicalType.hpp"

class MissionClient
{
public:

    MissionClient();

    unsigned int id;
    bool isComplete;
    LogicalType logicalType;
    std::map<unsigned int, MissionClient> childMissions;
    std::map<unsigned int, RequirementClient> requirements;

    void handleSnapshot(bit::ServerPacket &packet);

};

#endif