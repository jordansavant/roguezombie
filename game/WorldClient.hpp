#pragma once
#ifndef RZ_WORLDCLIENT_H
#define RZ_WORLDCLIENT_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Structures.hpp"
#include <map>

class ZombieClient;
class OgreClient;
class TileClient;
class WallClient;
class GameplayState;

class WorldClient : public sf::Drawable, public sf::Transformable
{
public:

    WorldClient();

    ~WorldClient();

    GameplayState* state;
    std::map<unsigned int, ZombieClient*> zombies;
    std::map<unsigned int, OgreClient*> ogres;
    std::map<unsigned int, TileClient*> tiles;
    std::map<unsigned int, WallClient*> walls;
    bit::Pool<TileClient> tilePool;
    bit::Pool<ZombieClient> zombiePool;
    bit::Pool<OgreClient> ogrePool;
    bit::Pool<WallClient> wallPool;
    sf::Font font;
    TileClient* hoveredTile;

    // Spritesheet 01;
    sf::Texture texture_spritesheet_01;
    bit::VertexMap vertexMap_01;

    void load(GameplayState* state);

    void update(sf::RenderWindow &window, sf::Time &gameTime);

    void handleSnapshot(bit::ServerPacket &packet, bool full = false);

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    template <class T>
    void diffNetworkEntity(std::map<unsigned int, T*> &map, bit::Pool<T> &pool)
    {
        auto itr = map.begin();
        while(itr != map.end())
        {
            if (itr->second->lastSnapshotId != state->lastSnapshotId)
            {
                pool.recycle(itr->second);
                map.erase(itr++);
            }
            else
            {
                ++itr;
            }
        }
    }

    template <class T>
    T* unpackNetworkEntity(bit::ServerPacket &packet, bool full, std::map<unsigned int, T*> &map, bit::Pool<T> &pool)
    {
        unsigned int id;
        packet >> id;

        T* entity;
        auto itr = map.find(id);
        if(itr != map.end())
        {
            entity = itr->second;
        }
        else
        {
            entity = pool.fetch();
            map.insert(std::pair<unsigned int, T*>(id, entity));
        }
        entity->lastSnapshotId = state->lastSnapshotId; // update snapshot id
        entity->handleSnapshot(packet, full);

        return entity;
    }
};

#endif