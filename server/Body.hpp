#pragma once
#ifndef RZ_BODY_H
#define RZ_BODY_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "Interaction.hpp"
#include <map>
#include <functional>

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
    Body* inventoryGuest; // Who is accessing my inventory
    Body* inventoryHost; // Whose inventory I am accessing
    Body* conversationSpeaker;
    std::map<unsigned int, DialogNode*> currentConversations;
    std::map<unsigned int, DialogNode*> originConversations; // for memory management
    std::function<DialogNode*(void)> getStartingDialog;

	struct Schema
	{
        Schema()
            : id(0), type(Type::None), x(0), y(0), width(0), height(0), illumination(0), obstructionRatio(0)
        {
        }

        unsigned int id;
        Type type;
		float x, y;
        float width, height;
        float illumination; // how illuminated this body is
        unsigned char rshade, gshade, bshade; // color of light
        float obstructionRatio;

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            packet << sf::Uint32(schema.id);
            packet << sf::Uint32(schema.type);
            packet << schema.x;
            packet << schema.y;
            packet << schema.width;
            packet << schema.height;
            packet << schema.illumination;
            packet << schema.rshade;
            packet << schema.gshade;
            packet << schema.bshade;
            packet << schema.obstructionRatio;
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
            packet >> schema.rshade;
            packet >> schema.gshade;
            packet >> schema.bshade;
            packet >> schema.obstructionRatio;
            return packet;
        }
	};
	Schema schema;

    virtual void load(Level* level, unsigned int id, Type type, float x, float y, float width, float height);

    virtual void update(sf::Time &gameTime);

    virtual void distributedUpdate(sf::Time &gameTime);

    void addItemToInventory(Item* item);

    Item* removeItemFromInventory(unsigned int itemId);

    void openInventoryForGuest(Body* guest);

    void closeInventoryForGuest();

    void openInventoryOfHost(Body* host);
    
    void closeInventoryOfHost();

    void handleDialogResponse(Body* listener, unsigned int responseId);

    virtual void handleInteraction(Interaction::Type interaction, Body* interactor, bit::ServerPacket &responsePacket);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual void getAvailableInteractions(std::vector<Interaction::Type> &fill);

    virtual void prepareDialogNode(Body* listener, bit::ServerPacket &packet);
    
};

#endif