#include "MissionClient.hpp"
#include "RequirementClient.hpp"
#include "../../bitengine/Network.hpp"

MissionClient::MissionClient()
    : isComplete(false), logicalType(LogicalType::Sequence)
{
}

void MissionClient::handleSnapshot(bit::ServerPacket &packet)
{
    packet >> isComplete;
    
    unsigned int logical_type_int;
    packet >> logical_type_int;
    logicalType = static_cast<LogicalType>(logical_type_int);
    
    unsigned int childMissionSize;
    packet >> childMissionSize;
    childMissions.resize(childMissionSize);
    for(unsigned int i=0; i < childMissions.size(); i++)
    {
        childMissions[i].handleSnapshot(packet);
    }

    unsigned int requirementsSize;
    packet >> requirementsSize;
    requirements.resize(requirementsSize);
    for(unsigned int i=0; i < requirements.size(); i++)
    {
        requirements[i].handleSnapshot(packet);
    }
}