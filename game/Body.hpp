#pragma once
#ifndef RZ_BODY_H
#define RZ_BODY_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "Interaction.hpp"
#include <map>

class Level;
class Item;
class DialogNode;

class Body
{
public:

    Body();

    ~Body();

    enum Type
    {
        None,
        Character,
        Structure
    };

    Level* level;
    bool blockFoV;
    Item* inventory;
    Body* inventoryAccessor;
    Body* inventoryAccessee;
    Body* conversationSpeaker;
    std::map<unsigned int, DialogNode*> currentConversations;
    std::map<unsigned int, DialogNode*> originConversations; // for memory management

	struct Schema
	{
        Schema()
            : id(0), type(Type::None), x(0), y(0), width(0), height(0), illumination(0)
        {
        }

        unsigned int id;
        Type type;
		float x, y;
        float width, height;
        float illumination;

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            packet << sf::Uint32(schema.id);
            packet << sf::Uint32(schema.type);
            packet << schema.x;
            packet << schema.y;
            packet << schema.width;
            packet << schema.height;
            packet << schema.illumination;
            return packet;
        }

        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            packet >> schema.id;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Body::Type>(packet, schema.type);
            packet >> schema.x;
            packet >> schema.y;
            packet >> schema.width;
            packet >> schema.height;
            packet >> schema.illumination;
            return packet;
        }
	};
	Schema schema;

    virtual void load(Level* level, unsigned int id, Type type, float x, float y, float width, float height);

    virtual void update(sf::Time &gameTime);

    void addItemToInventory(Item* item);

    Item* removeItemFromInventory(unsigned int itemId);

    virtual DialogNode* getDefaultDialogNode();

    void handleDialogResponse(Body* listener, unsigned int responseId);

    virtual void handleInteraction(Interaction::Type interaction, Body* interactor, bit::ServerPacket &responsePacket);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual void prepareInteractionTree(bit::ServerPacket &packet);

    virtual void prepareDialogNode(Body* listener, bit::ServerPacket &packet);
    
};

#endif