#pragma once
#ifndef RZ_WORLDCLIENT_H
#define RZ_WORLDCLIENT_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Structures.hpp"
#include "GameplayState.hpp"
#include "Minimap.hpp"
#include <map>

class BaseLevelClientRunner;
class ZombieClient;
class OgreClient;
class TileClient;
class WallClient;
class DoorClient;
class Character;

class LevelClient : public sf::Drawable, public sf::Transformable
{
public:

    LevelClient();

    virtual ~LevelClient();

    GameplayState* state;
    unsigned int levelId;
    std::map<unsigned int, ZombieClient*> zombies;
    std::map<unsigned int, OgreClient*> ogres;
    std::map<unsigned int, TileClient*> tiles;
    std::map<unsigned int, WallClient*> walls;
    std::map<unsigned int, DoorClient*> doors;
    bit::Pool<TileClient> tilePool;
    bit::Pool<ZombieClient> zombiePool;
    bit::Pool<OgreClient> ogrePool;
    bit::Pool<WallClient> wallPool;
    bit::Pool<DoorClient> doorPool;
    TileClient* hoveredTile;
    Character* playerCharacter;
    Minimap minimap;

    // Spritesheet 01;
    sf::Texture texture_spritesheet_01_unsmooth;
    sf::Texture texture_spritesheet_01_smooth;
    bit::VertexMap vertexMap_01;

    // logical
    std::vector<BaseLevelClientRunner*> runners;

    void load(GameplayState* state);

    void update(sf::Time &gameTime);

    void handleSnapshot(bit::ServerPacket &packet, bool full = false);

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

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    
};

#endif