#include "MissionClient.hpp"
#include "../../bitengine/Network.hpp"

MissionClient::MissionClient()
    : id(0), isComplete(false), logicalType(LogicalType::Sequence), journalEntry(JournalEntry::Entry::None)
{
}

void MissionClient::handleSnapshot(bit::ServerPacket &packet)
{
    packet >> isComplete;

    unsigned int logical_type_int;
    packet >> logical_type_int;
    logicalType = static_cast<LogicalType>(logical_type_int);

    unsigned int journal_entry_int;
    packet >> journal_entry_int;
    journalEntry = static_cast<JournalEntry::Entry>(journal_entry_int);

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