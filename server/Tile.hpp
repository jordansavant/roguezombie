#pragma once
#ifndef RZ_TILE_H
#define RZ_TILE_H

#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Intelligence.hpp"
#include "../bitengine/Structures.hpp"
#include <functional>
#include "LevelRunnable.hpp"

class Level;
class Body;

class Tile : public bit::Node, public LevelRunnable
{
public:

    Tile();

    ~Tile();

    enum Type
    {
        None,
        Ground,
    };

    struct Entrance
    {
        unsigned int id;
        unsigned int priority;
    };

    struct Schema
    {
        Schema()
            : id(0), x(0), y(0), centerX(0), centerY(0), width(0), height(0), type(Type::Ground), bodyId(0), doorId(0), illumination(0), gshade(0), bshade(0)
        {
        }

        unsigned int id; // is map index too
        float x, y;
        float centerX, centerY;
        unsigned int width, height;
        Type type;
        unsigned int bodyId;
        unsigned int doorId;
        float illumination; // how illuminated this tile is
        unsigned char rshade, gshade, bshade; // color of light

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            packet << sf::Uint32(schema.id);
            packet << schema.x;
            packet << schema.y;
            packet << schema.centerX;
            packet << schema.centerY;
            packet << sf::Uint32(schema.width);
            packet << sf::Uint32(schema.height);
            packet << sf::Uint32(schema.type);
            packet << sf::Uint32(schema.bodyId);
            packet << sf::Uint32(schema.doorId);
            packet << schema.illumination;
            packet << schema.rshade;
            packet << schema.gshade;
            packet << schema.bshade;
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            packet >> schema.id;
            packet >> schema.x;
            packet >> schema.y;
            packet >> schema.centerX;
            packet >> schema.centerY;
            packet >> schema.width;
            packet >> schema.height;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Tile::Type>(packet, schema.type);
            packet >> schema.bodyId;
            packet >> schema.doorId;
            packet >> schema.illumination;
            packet >> schema.rshade;
            packet >> schema.gshade;
            packet >> schema.bshade;
            return packet;
        }
    };

    Schema schema;
    Level* level;
    Body* body;
    Body* door;
    std::vector<Entrance> entrances;
    unsigned int metadata_shadowcastId;
    unsigned int metadata_floodfillId;
    bit::Event<std::function<void(Tile* t, Body* body)>> onBodyEnter;
    bit::Event<std::function<void(Tile* t, Body* body)>> onBodyLeave;

    virtual void load(Level* level, unsigned int id, Type type, int x, int y, int width, int height);

    virtual void update(sf::Time &gameTime);

    virtual void distributedUpdate(sf::Time &gameTime);

    virtual void setOccupyingBody(Body* body);

    virtual void setOccupyingDoor(Body* body);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    bool isOccupied();

    bool hasEntrance(unsigned int entranceId);

    unsigned int getEntrancePriority(unsigned int entranceId);

private:

    void runOnBodyEnter(Body* body);

    void runOnBodyLeave(Body* body);
};

#endif