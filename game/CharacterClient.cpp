#include "CharacterClient.hpp"

CharacterClient::CharacterClient()
    : schema()
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
}

void CharacterClient::handleServerEventPacket_missionCompleted(bit::ServerPacket &packet)
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
        mc->schema.isComplete = true;
        level->state->displayMessage(std::string(JournalEntry::getTitle(mc->schema.journalEntry) + " completed"));
    }
}

void CharacterClient::handleServerEventPacket_itemAdded(bit::ServerPacket &packet)
{
    // depth includes all parents and the new item id
    unsigned int depth;
    packet >> depth;

    unsigned int inventory_id;
    packet >> inventory_id;
    ItemClient* ic = &inventoryClient;
    // Adding an item to the inventory normally
    // skip the first and last ID as that is our inventory then new item
    for(unsigned int i=1; i < depth - 1; i++)
    {
        unsigned int itemId;
        packet >> itemId;
        ic = &inventoryClient.itemClients[itemId];
    }

    if(ic)
    {
        unsigned int itemId;
        packet >> itemId;

        // unpack the item into it
        inventoryClient.itemClients[itemId] = ItemClient();
        inventoryClient.itemClients[itemId].handleSnapshot(packet);
    }
}