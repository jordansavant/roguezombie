#include "MissionClient.hpp"
#include "../../bitengine/Network.hpp"

MissionClient::MissionClient()
    : schema()
{
}

void MissionClient::handleSnapshot(bit::ServerPacket &packet)
{
    packet >> schema.isComplete;
    bit::NetworkHelper::unpackEnum<sf::Uint32, LogicalType>(packet, schema.logicalType);
    bit::NetworkHelper::unpackEnum<sf::Uint32, JournalEntry::Entry>(packet, schema.journalEntry);

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