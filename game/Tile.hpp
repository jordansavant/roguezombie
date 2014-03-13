#pragma once
#ifndef RZ_TILE_H
#define RZ_TILE_H

#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Intelligence.hpp"
#include <functional>

class World;
class Body;

class Tile : public bit::NodeContainer
{
public:

    Tile();

    ~Tile();

    enum Type
    {
        Ground,
    };

    World* world;
    Body* body;
    Body* door;
    unsigned int metadata_shadowcastId;

    std::vector<std::function<void(Tile* t, Body* body)>> onBodyEnter;
    std::vector<std::function<void(Tile* t, Body* body)>> onBodyLeave;

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
        unsigned int doorId;
        float illumination; // how illuminated this tile is
        unsigned char rshade, gshade, bshade; // color of light

        friend sf::Packet& operator <<(sf::Packet& packet, const DeltaState &state)
        {
            return packet << sf::Uint32(state.bodyId) << sf::Uint32(state.doorId) << state.illumination << state.rshade << state.gshade << state.bshade;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, DeltaState &state)
        {
            return packet >> state.bodyId >> state.doorId >> state.illumination >> state.rshade >> state.gshade >> state.bshade;
        }
    };
    DeltaState deltaState;

    virtual void load(World* world, unsigned int id, Type type, int x, int y, int width, int height);

    virtual void update(sf::Time &gameTime);

    virtual void setOccupyingBody(Body* body);

    virtual void setOccupyingDoor(Body* body);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);

private:

    void runOnBodyEnter(Body* body);

    void runOnBodyLeave(Body* body);

};

#endif