#include "RequirementClient.hpp"

RequirementClient::RequirementClient()
    : isFullfilled(false)
{
}

void RequirementClient::handleSnapshot(bit::ServerPacket &packet)
{
    packet >> isFullfilled;
}