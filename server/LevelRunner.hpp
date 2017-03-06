#pragma once
#ifndef RZ_LEVELRUNNER_H
#define RZ_LEVELRUNNER_H

#include "SFML/Graphics.hpp"
#include <map>
#include <deque>

class Level;
class Character;

class BaseLevelRunner
{
public:
    BaseLevelRunner() : distributedUpdateCounter(0) { }

    virtual ~BaseLevelRunner() { }

    virtual void update(sf::Time &gameTime) = 0;

    unsigned int distributedUpdateCounter;
};

template <class T>
class LevelRunner : public BaseLevelRunner
{
public:
    LevelRunner(Level* _level, std::vector<T*>* _list)
        : BaseLevelRunner(), level(_level), list(_list)
    {
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
        // Update all runners
        //for(unsigned int i=0; i < list->size(); i++)
        //{
        //    (*list)[i]->update(gameTime);
        //
        //    if(distributedUpdateCounter == i)
        //    {
        //        (*list)[i]->distributedUpdate(gameTime);
        //    }
        //}

        // Update with allowance for adding / removing entities mid loop
        unsigned int i = 0;
        for(auto it = list->begin(); it != list->end();)
        {
            (*it)->update(gameTime);

            if(distributedUpdateCounter == i)
            {
                (*it)->distributedUpdate(gameTime);
            }

            i++; // iterate tracking index
            
            // See if we need to remove the element
            if ((*it)->removeFromLevel)
            {
                delete (*it);
                it = list->erase(it);
            }
            else
            {
                ++it; // iterate list pointer
            }
        }

        // Support for load distributed updates
        distributedUpdateCounter++;
        if(distributedUpdateCounter >= list->size())
        {
            distributedUpdateCounter = 0;
        }

    }
};

#endif