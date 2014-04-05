#pragma once
#ifndef RZ_WORLDRUNNER_H
#define RZ_WORLDRUNNER_H

#include "SFML/Graphics.hpp"
#include <map>

class Level;

class BaseLevelRunner
{
public:
    BaseLevelRunner() { }

    virtual ~BaseLevelRunner() { }

    virtual void update(sf::Time &gameTime) = 0;

};


template <class T>
class LevelRunner : public BaseLevelRunner
{
public:
    LevelRunner(Level* _level, std::vector<T*>* _list)
        : BaseLevelRunner()
    {
        level = _level;
        list = _list;
    }

    virtual ~LevelRunner()
    {
        for(unsigned int i=0; i < list->size(); i++)
        {
            T* t = (*list)[i];
            delete t;
        }
    }

    Level* level;
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