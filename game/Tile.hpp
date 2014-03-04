#pragma once
#ifndef RZ_TILE_H
#define RZ_TILE_H

#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"

class World;
class Body;

class Tile
{
public:

    Tile();

    enum Type
    {
        Ground,
    };

    World* world;
    Body* body;
    
    struct FixedState
    {
        unsigned int id;
        float x, y;
        float centerX, centerY;
        unsigned int width, height;
        Type type;

        friend sf::Packet& operator <<(sf::Packet& packet, const FixedState &state)
        {
            packet << 
            sf::Uint32(state.id) << 
            state.x << 
            state.y << 
            state.centerX << 
            state.centerY << 
            sf::Uint32(state.width) <<
            sf::Uint32(state.height) <<
            sf::Uint32(state.type);
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, FixedState &state)
        {
            sf::Uint32 type;
            packet >>
            state.id >> 
            state.x >> 
            state.y >> 
            state.centerX >> 
            state.centerY >> 
            state.width >>
            state.height >>
            type;
            state.type = static_cast<Tile::Type>(type);
            return packet;
        }
    };
    FixedState fixedState;

    struct DeltaState
    {
        unsigned int bodyId;

        friend sf::Packet& operator <<(sf::Packet& packet, const DeltaState &state)
        {
            return packet << sf::Uint32(state.bodyId);
        }
        friend sf::Packet& operator >>(sf::Packet& packet, DeltaState &state)
        {
            return packet >> state.bodyId;
        }
    };
    DeltaState deltaState;


    virtual void load(World* world, unsigned int id, Type type, int x, int y, int width, int height);

    virtual void update(sf::Time &gameTime);

    virtual void setOccupyingBody(Body* body);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif