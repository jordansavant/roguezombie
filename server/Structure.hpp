#pragma once
#ifndef RZ_STRUCTURE_H
#define RZ_STRUCTURE_H

#include "Body.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"

class Level;
class Player;
class Tile;

class Structure : public Body
{
public:

    Structure();

    bool isDestroyable;

    enum Type
    {
        None, // 0
        Wall, // 1
        Door, // 2
        Chest, // 3 
        Furnishing // 4
    };

    struct Schema
    {
        Schema()
            :type(Type::None), interactable(false)
        {
        }

        Type type;
        bool interactable;

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            packet << sf::Uint32(schema.type);
            packet << schema.interactable;
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            bit::NetworkHelper::unpackEnum<sf::Uint32, Structure::Type>(packet, schema.type);
            packet >> schema.interactable;
            return packet;
        }
    };
    Schema schema;

    virtual void load(Level* level, unsigned int id, Type type, float x, float y, float width, float height);

    virtual void update(sf::Time &gameTime);

    virtual void setPosition(float x, float y);

    void getAvailableInteractions(std::vector<Interaction::Type> &fill);

    virtual void handleInteraction(Interaction::Type interaction, Body* interactor, bit::ServerPacket &responsePacket);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    static std::string getTitle(Structure::Type type);
};

#endif