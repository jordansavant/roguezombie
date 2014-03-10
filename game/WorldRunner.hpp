#pragma once
#ifndef RZ_WORLDRUNNER_H
#define RZ_WORLDRUNNER_H

#include "SFML/Graphics.hpp"
#include <map>

class World;

class BaseWorldRunner
{
public:
    BaseWorldRunner() { }

    virtual ~BaseWorldRunner() { }

    virtual void update(sf::Time &gameTime) = 0;

};


template <class T>
class WorldRunner : public BaseWorldRunner
{
public:
    WorldRunner(World* _world, std::vector<T*>* _list)
        : BaseWorldRunner()
    {
        world = _world;
        list = _list;
    }

    virtual ~WorldRunner()
    {
        for(unsigned int i=0; i < list->size(); i++)
        {
            T* t = (*list)[i];
            delete t;
        }
    }

    World* world;
    std::vector<T*>* list;

    virtual void update(sf::Time &gameTime)
    {
        for(unsigned int i=0; i < list->size(); i++)
        {
            (*list)[i]->update(gameTime);
        }
    }

};

#endif