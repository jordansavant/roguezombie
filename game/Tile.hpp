#pragma once
#ifndef RZ_TILE_H
#define RZ_TILE_H

#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"

class World;

class Tile
{
public:

    Tile();

    enum Type
    {
        Ground,
    };
    
    struct FixedState
    {
        int x, y;
        int centerX, centerY;
        int width, height;
        Type type;
        friend sf::Packet& operator <<(sf::Packet& packet, const FixedState &state)
        {
            return packet << 
                sf::Int32(state.x) << 
                sf::Int32(state.y) << 
                sf::Int32(state.centerX) << 
                sf::Uint32(state.centerY) << 
                sf::Uint32(state.width) <<
                sf::Uint32(state.height) <<
                sf::Uint32(state.type);
        }
        friend sf::Packet& operator >>(sf::Packet& packet, FixedState &state)
        {
            sf::Uint32 typeNum;
            packet >>
            state.x >> 
            state.y >> 
            state.centerX >> 
            state.centerY >> 
            state.width >>
            state.height >>
            typeNum;
            state.type = static_cast<Tile::Type>(typeNum);
            return packet;
        }
    };
    FixedState fixedState;

    struct DeltaState
    {
        friend sf::Packet& operator <<(sf::Packet& packet, const DeltaState &state)
        {
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, DeltaState &state)
        {
            return packet;
        }
    };
    DeltaState deltaState;

    World* world;

    virtual void load(World* world, Type type, int x, int y, int width, int height);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif