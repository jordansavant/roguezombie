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
    BaseLevelRunner() { }

    virtual ~BaseLevelRunner() { }

    virtual void update(sf::Time &gameTime) = 0;
};

template <class T>
class LevelRunner : public BaseLevelRunner
{
public:
    LevelRunner(Level* _level, std::vector<T*>* _list)
        : BaseLevelRunner(), level(_level), list(_list), distributedUpdateCounter(0)
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
    unsigned int distributedUpdateCounter;

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
            ++it; // iterate list pointer
            //if ((*it)->removeFromLevel)
            //{
            //    delete (*it);
            //    it = childElements.erase(it);
            //}
            //else
            //{
            //    ++it;
            //}
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