#pragma once
#ifndef RZ_WORLDCLIENTRUNNER_H
#define RZ_WORLDCLIENTRUNNER_H

#include "SFML/Graphics.hpp"
#include <map>

class WorldClient;

class BaseWorldClientRunner
{
public:
    BaseWorldClientRunner() { }

    virtual ~BaseWorldClientRunner() { }

    virtual void buildPool() = 0;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime) = 0;

    virtual void diffNetwork() = 0;
};


template <class T>
class WorldClientRunner : public BaseWorldClientRunner
{
public:
    WorldClientRunner(WorldClient* _world, std::map<unsigned int, T*>* _map, bit::Pool<T>* _pool, unsigned int _poolCount)
        : BaseWorldClientRunner()
    {
        world = _world;
        map = _map;
        pool = _pool;
        poolCount = _poolCount;
    }

    virtual ~WorldClientRunner()
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
        WorldClient* w = world;
        pool->factoryMethod = [w] () -> T* {
        T* t = new T();
            t->clientLoad(w);
            return t;
        };
        pool->add(poolCount);
    }

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime)
    {
        for(auto iterator = map->begin(); iterator != map->end(); iterator++)
        {
            iterator->second->clientUpdate(window, gameTime);
        }
    }

    virtual void diffNetwork()
    {
        auto itr = map->begin();
        while(itr != map->end())
        {
            if (itr->second->lastSnapshotId != world->state->lastSnapshotId)
            {
                pool->recycle(itr->second);
                map->erase(itr++);
            }
            else
            {
                ++itr;
            }
        }
    }
};

#endif