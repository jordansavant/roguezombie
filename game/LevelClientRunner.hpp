#pragma once
#ifndef RZ_WORLDCLIENTRUNNER_H
#define RZ_WORLDCLIENTRUNNER_H

#include "SFML/Graphics.hpp"
#include <map>

class LevelClient;

class BaseLevelClientRunner
{
public:
    BaseLevelClientRunner() { }

    virtual ~BaseLevelClientRunner() { }

    virtual void buildPool() = 0;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime) = 0;

    virtual void diffNetwork() = 0;
};


template <class T>
class LevelClientRunner : public BaseLevelClientRunner
{
public:
    LevelClientRunner(LevelClient* _level, std::map<unsigned int, T*>* _map, bit::Pool<T>* _pool, unsigned int _poolCount)
        : BaseLevelClientRunner()
    {
        level = _level;
        map = _map;
        pool = _pool;
        poolCount = _poolCount;
    }

    virtual ~LevelClientRunner()
    {
        for(auto iterator = map->begin(); iterator != map->end(); iterator++)
        {
            delete iterator->second;
        }
        map->clear();
    }

    LevelClient* level;
    std::map<unsigned int, T*>* map;
    bit::Pool<T>* pool;
    unsigned int poolCount;

    virtual void buildPool()
    {
        LevelClient* w = level;
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
            if (itr->second->lastSnapshotId != level->state->lastSnapshotId)
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