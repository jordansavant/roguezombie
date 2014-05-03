#include "Body.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "GameplayServer.hpp"
#include "Level.hpp"
#include "Tile.hpp"
#include "Character.hpp"
#include "items/Item.hpp"

Body::Body()
    : level(NULL), blockFoV(true), schema(), inventory(NULL), inventoryAccessor(NULL), inventoryAccessee(NULL)
{
}

Body::~Body()
{
    delete inventory;
}

void Body::load(Level* _level, unsigned int _id, Type _type, float _x, float _y, float _width, float _height)
{
    level = _level;
    schema.id = _id;
    schema.type = _type;
    schema.x = _x;
    schema.y = _y;
    schema.width = _width;
    schema.height = _height;

    inventory = Item::create(Item::Type::Backpack);
    inventory->parentBody = this;
    inventory->schema.id = level->server->getNextItemId();
}

void Body::update(sf::Time &gameTime)
{
}

void Body::addItemToInventory(Item* item)
{
    inventory->addItem(item);
}

Item* Body::removeItemFromInventory(unsigned int itemId)
{
    Item* item = inventory->removeItem(itemId);
    if(item)
    {
        item->parentBody = NULL;
    }
    return item;
}

void Body::handleInteraction(Interaction::Type interaction, Body* interactor, bit::ServerPacket &responsePacket)
{
    switch(interaction)
    {
        case Interaction::Type::OpenInventory:
        {
            // If someone else is already accessing my inventory, deny
            if(inventoryAccessor && inventoryAccessor != interactor)
            {
                responsePacket << false;
            }
            else
            {
                inventoryAccessor = interactor;
                interactor->inventoryAccessee = this;
                responsePacket << true;
                inventory->prepareSnapshot(responsePacket);
            }
            break;
        }
        case Interaction::Type::CloseInventory:
        {
            // Unset myself from their access perspective
            if(inventoryAccessor)
                inventoryAccessor->inventoryAccessee = NULL;
            inventoryAccessor = NULL;
            break;
        }
    }
}

void Body::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    packet << schema;

    // Items
    packet << full;
    if(full)
    {
        inventory->prepareSnapshot(packet);
    }
}

void Body::prepareInteractionTree(bit::ServerPacket &packet)
{
    packet << sf::Uint32(0);
}