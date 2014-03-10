#pragma once
#ifndef RZ_WORLDCLIENT_H
#define RZ_WORLDCLIENT_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Structures.hpp"
#include "Minimap.hpp"
#include <map>


class Baserunner
{
public:
    Baserunner() { }

    virtual ~Baserunner() { }

    virtual void buildPool() = 0;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime) = 0;

    virtual void diffNetwork() = 0;
};


template <class T>
class Runner : public Baserunner
{
public:
    Runner(WorldClient* _world, std::map<unsigned int, T*>* _map, bit::Pool<T>* _pool, unsigned int _poolCount)
        : Baserunner()
    {
        world = _world;
        map = _map;
        pool = _pool;
        poolCount = _poolCount;
    }

    virtual ~Runner()
    {
        for(unsigned int i=0; i < map->size(); i++)
        {
            T* t = (*map)[i];
            delete t;
        }
    }

    WorldClient* world;
    std::map<unsigned int, T*>* map;
    bit::Pool<T>* pool;
    unsigned int poolCount;

    virtual void buildPool()
    {
        world->setupEntityPool(*pool, poolCount);
    }

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime)
    {
        world->updateEntity(*map, window, gameTime);
    }

    virtual void diffNetwork()
    {
        world->diffNetworkEntity(*map, *pool);
    }
};


class ZombieClient;
class OgreClient;
class TileClient;
class WallClient;
class DoorClient;
class Character;
class GameplayState;

class WorldClient : public sf::Drawable, public sf::Transformable
{
public:

    WorldClient();

    virtual ~WorldClient();

    GameplayState* state;
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
    std::vector<Baserunner*> runners;

    void load(GameplayState* state);

    void update(sf::RenderWindow &window, sf::Time &gameTime);

    void handleSnapshot(bit::ServerPacket &packet, bool full = false);

    template <class T>
    void setupEntityPool(bit::Pool<T> &pool, unsigned int count)
    {
        WorldClient* w = this;
        pool.factoryMethod = [w] () -> T* {
        T* t = new T();
            t->clientLoad(w);
            return t;
        };
        pool.add(count);
    }

    template <class T>
    void updateEntity(std::map<unsigned int, T*> &map, sf::RenderWindow &window, sf::Time &gameTime)
    {
        for(auto iterator = map.begin(); iterator != map.end(); iterator++)
        {
            iterator->second->clientUpdate(window, gameTime);
        }
    }

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

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
    
};

#endif