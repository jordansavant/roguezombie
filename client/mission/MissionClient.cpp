#include "MissionClient.hpp"
#include "../../bitengine/Network.hpp"

MissionClient::MissionClient()
    : schema()
{
}

void MissionClient::handleSnapshot(bit::ServerPacket &packet)
{
    packet >> schema;

    unsigned int childMissionSize;
    packet >> childMissionSize;
    childMissions.clear();
    for(unsigned int i=0; i < childMissionSize; i++)
    {
        unsigned int missionId;
        packet >> missionId;
        childMissions[missionId] = MissionClient();
        childMissions[missionId].handleSnapshot(packet);
    }
}