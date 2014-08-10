#include "Body.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "GameplayServer.hpp"
#include "Level.hpp"
#include "Tile.hpp"
#include "Character.hpp"
#include "Light.hpp"
#include "items/Item.hpp"
#include "dialog/DialogNode.hpp"

Body::Body()
    : level(NULL), blockFoV(true), schema(), inventory(NULL), inventoryGuest(NULL), inventoryHost(NULL), conversationSpeaker(NULL), getStartingDialog(NULL)
{
}

Body::~Body()
{
    delete inventory;

    for(auto itr = originConversations.begin(); itr != originConversations.end(); itr++)
    {
        delete itr->second;
    }
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
    schema.illumination = 0;
    schema.rshade = 0;
    schema.gshade = 0;
    schema.bshade = 0;

    inventory = Item::create(Item::Type::Backpack, level->server->getNextItemId());
    inventory->parentBody = this;
}

void Body::update(sf::Time &gameTime)
{
    // Reset brightness to none
    schema.illumination = 0.0f;

    // Reset color to black
    schema.rshade = 0;
    schema.gshade = 0;
    schema.bshade = 0;

    // If I have a light, update it
    for(unsigned int i=0; i < lights.size(); i++)
    {
        lights[i]->x = Body::schema.x;
        lights[i]->y = Body::schema.y;
    }
}

void Body::distributedUpdate(sf::Time &gameTime)
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

void Body::openInventoryForGuest(Body* guest)
{
    // Set the guest in my inventory
    inventoryGuest = guest;

    // Tell the guest I am his host
    guest->inventoryHost = this;
}

void Body::closeInventoryForGuest()
{
    // If I have a guest
    if(inventoryGuest)
    {
        // Tell the guest I am not hosting
        inventoryGuest->inventoryHost = NULL;

        // Tell myself my guest is gone
        inventoryGuest = NULL;
    }
}

void Body::openInventoryOfHost(Body* host)
{
    // Set the host as my host
    inventoryHost = host;

    // Tell the host I am his guest
    host->inventoryGuest = this;
}

void Body::closeInventoryOfHost()
{
    // If I have a host
    if(inventoryHost)
    {
        // Tell the host I am leaving
        inventoryHost->inventoryGuest = NULL;

        // Tell myself I have no host
        inventoryHost = NULL;
    }
}

void Body::handleDialogResponse(Body* listener, unsigned int responseId)
{
    DialogNode* node = currentConversations[listener->schema.id];
    if(node)
    {
        DialogNode* nextNode = node->chooseResponse(this, listener, responseId);
        if(nextNode)
        {
            currentConversations[listener->schema.id] = nextNode;
        }
    }
}

void Body::handleInteraction(Interaction::Type interaction, Body* interactor, bit::ServerPacket &responsePacket)
{
    switch(interaction)
    {
        case Interaction::Type::OpenInventory:
        {
            // If someone else is already accessing my inventory, deny
            if(inventoryGuest && inventoryGuest != interactor)
            {
                responsePacket << false;
            }
            else
            {
                openInventoryForGuest(interactor);
                responsePacket << true;
                inventory->prepareSnapshot(responsePacket);
            }
            break;
        }
        case Interaction::Type::CloseInventory:
        {
            // Unset myself from their access perspective
            if(inventoryGuest)
                inventoryGuest->inventoryHost = NULL;
            inventoryGuest = NULL;
            break;
        }
        case Interaction::Type::Dialog:
        {
            interactor->conversationSpeaker = this;
            prepareDialogNode(interactor, responsePacket);
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

void Body::getAvailableInteractions(std::vector<Interaction::Type> &fill)
{
    if(getStartingDialog)
    {
        fill.push_back(Interaction::Type::Dialog);
    }
}

void Body::prepareDialogNode(Body* listener, bit::ServerPacket &packet)
{
    DialogNode* node = NULL;
    auto itr = currentConversations.find(listener->schema.id);
    if(itr == currentConversations.end())
    {
        node = getStartingDialog();
        originConversations[listener->schema.id] = node;
        currentConversations[listener->schema.id] = node;
    }
    else
    {
        node = itr->second;
    }

    node->prepareSnapshot(packet);

    // FUTURE - pack conversation modifiers such disposition etc.
}