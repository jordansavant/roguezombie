#pragma once
#ifndef RZ_WORLDCLIENTRUNNER_H
#define RZ_WORLDCLIENTRUNNER_H

#include "SFML/Graphics.hpp"
#include "LevelClient.hpp"
#include <map>

class LevelClient;

class BaseLevelClientRunner
{
public:
    BaseLevelClientRunner() { }

    virtual ~BaseLevelClientRunner() { }

    virtual void buildPool() = 0;

    virtual void update(sf::Time &gameTime) = 0;

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

    virtual void update(sf::Time &gameTime)
    {
        for(auto iterator = map->begin(); iterator != map->end(); iterator++)
        {
            iterator->second->clientUpdate(gameTime);
        }
    }

    virtual void diffNetwork()
    {
        // Clear the map of any entities that are no longer in client's state
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

template <class T>
class LevelClientTileRunner : public LevelClientRunner<T>
{
public:
    LevelClientTileRunner(LevelClient* _level, std::map<unsigned int, T*>* _map, bit::Pool<T>* _pool, unsigned int _poolCount, std::vector<T*>* _vector)
        : LevelClientRunner<T>(_level, _map, _pool, _poolCount), vector(_vector), mymap(_map), mylevel(_level), mypool(_pool)
    {
    }

	std::map<unsigned int, T*>* mymap;
	LevelClient* mylevel;
	bit::Pool<T>* mypool;

    virtual ~LevelClientTileRunner()
    {
        vector->clear();
    }

    std::vector<T*>* vector;

    virtual void diffNetwork()
    {
        // Clear the map of any entities that are no longer in client's state
        auto itr = mymap->begin();
        while(itr != mymap->end())
        {
            if (itr->second->lastSnapshotId != mylevel->state->lastSnapshotId)
            {
                vector->at(itr->second->schema.id) = NULL;
				mypool->recycle(itr->second);
				mymap->erase(itr++);
            }
            else
            {
                ++itr;
            }
        }
    }

};

#endif