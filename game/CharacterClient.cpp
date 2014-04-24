#include "CharacterClient.hpp"

CharacterClient::CharacterClient()
    : schema(), backpackClient()
{
}

void CharacterClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    // Body
    BodyClient::handleSnapshot(packet, full);

    // Character
    packet >> schema;

    // Mission Clientside
    bool hasMissionUpdate;
    packet >> hasMissionUpdate;
    if(hasMissionUpdate)
    {
        unsigned int missionClientSize;
        packet >> missionClientSize;
        if(missionClientSize > 0)
        {
            missionClients.clear();
            for(unsigned int i=0; i < missionClientSize; i++)
            {
                unsigned int missionId;
                packet >> missionId;
                missionClients[missionId] = MissionClient();
                missionClients[missionId].handleSnapshot(packet);
            }
        }
    }

    // Item Clientside
    bool hasBackpack;
    packet >> hasBackpack;
    if(hasBackpack)
    {
        backpackClient.handleSnapshot(packet);
    }
}

void CharacterClient::handleMissionCompleteGameEvent(bit::ServerPacket &packet)
{
    unsigned int depth;
    packet >> depth;

    MissionClient* mc = NULL;
    for(unsigned int i=0; i < depth; i++)
    {
        unsigned int missionId;
        packet >> missionId;

        if(!mc)
            mc = &missionClients[missionId];
        else
            mc = &mc->childMissions[missionId];
    }

    if(mc)
    {
        mc->isComplete = true;
    }
}

void CharacterClient::handleItemAddGameEvent(bit::ServerPacket &packet)
{
    // depth includes all parents and the new item id
    unsigned int depth;
    packet >> depth;

    unsigned int backpack_id;
    packet >> backpack_id;
    ItemClient* ic = &backpackClient;
    // Adding an item to the inventory normally
    // skip the first and last ID as that is our backpack then new item
    for(unsigned int i=1; i < depth - 1; i++)
    {
        unsigned int itemId;
        packet >> itemId;
        ic = &backpackClient.itemClients[itemId];
    }

    if(ic)
    {
        unsigned int itemId;
        packet >> itemId;

        // unpack the item into it
        backpackClient.itemClients[itemId] = ItemClient();
        backpackClient.itemClients[itemId].handleSnapshot(packet);
    }
}
