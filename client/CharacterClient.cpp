#include "CharacterClient.hpp"
#include "LevelClient.hpp"
#include "StateGamePlay.hpp"
#include "RogueZombieGame.hpp"

CharacterClient::CharacterClient()
    : schema(), sprite(0, 0, 0, 0), chanceOfHit(0)
{
    hasEquipment.resize(Character::EquipmentSlot::_count, false);
    equipment.resize(Character::EquipmentSlot::_count);
}

void CharacterClient::clientLoad(LevelClient* _level)
{
    level = _level;

    sprite.load(this, level->state->rogueZombieGame->spriteLoader, level->vertexMap_charactersToggleIlluminated, level->vertexMap_charactersNormal);
}

void CharacterClient::clientUpdate(sf::Time &gameTime)
{
    sprite.update(gameTime);
}

void CharacterClient::reinitialize()
{
    switch(schema.type)
    {
        case Character::Type::Hunter:
        case Character::Type::Zombie:
            sprite.width = 40;
            sprite.height = 40;
            sprite.baseOffsetX = 20;
            sprite.baseOffsetY = 35;
            break;
        case Character::Type::Ogre:
            sprite.width = 120;
            sprite.height = 116;
            sprite.baseOffsetX = 48;
            sprite.baseOffsetY = 96;
            break;
    }

    sprite.syncSprites();
}

void CharacterClient::reset()
{
    sprite.reset();
}

void CharacterClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    // Body
    BodyClient::handleSnapshot(packet, full);

    // Character
    packet >> schema;

    // Equipment
    for(unsigned int i=0; i < Character::EquipmentSlot::_count; i++)
    {
        bool has;
        packet >> has;
        hasEquipment[i] = has;
        if(has)
        {
            equipment[i].handleSnapshot(packet);
        }
    }

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

    // Sprite
    sprite.syncronizeEquipment();
    sprite.cleanRender = full;
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
        level->state->displayPlayerMessage(this, std::string(JournalEntry::getTitle(mc->schema.journalEntry) + " completed"));
    }
}

void CharacterClient::handleServerEventPacket_inventoryUpdate(bit::ServerPacket &packet)
{
    inventoryClient.handleSnapshot(packet);
}

void CharacterClient::handleServerEventPacket_itemUpdated(bit::ServerPacket &packet)
{
    ItemClient ic;
    ic.handleSnapshot(packet);

    auto result = inventoryClient.itemClients.find(ic.schema.id);
    if(result != inventoryClient.itemClients.end())
    {
        inventoryClient.itemClients[ic.schema.id] = ic;
        level->state->displayPlayerMessage(this, std::string(Item::getTitle(ic.schema.type) + " updated"));
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
        level->state->displayPlayerMessage(this, std::string(Item::getTitle(inventoryClient.itemClients[itemId].schema.type) + " gained"));
    }
}

void CharacterClient::handleServerEventPacket_itemRemoved(bit::ServerPacket &packet)
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

        // Remove the item
        level->state->displayPlayerMessage(this, std::string(Item::getTitle(inventoryClient.itemClients[itemId].schema.type) + " removed"));
        inventoryClient.itemClients.erase(itemId);
    }
}