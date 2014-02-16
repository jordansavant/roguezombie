#include "LevelManager.hpp"
#include "GameComponent.hpp"
#include "Level.hpp"
#include <vector>

bit::LevelManager::LevelManager()
{
    currentLevel = NULL;
    nextLevel = NULL;
    pushLevel = NULL;
    popLevel = NULL;
}

bit::LevelManager::~LevelManager()
{
    // Delete levels
    for(unsigned int i = 0; i < levels.size(); i++)
    {
        if(levels[i] != NULL)
        {
            delete levels[i];
        }
    }

    if(pushLevel != NULL)
    {
        delete pushLevel;
    }
    if(nextLevel != NULL)
    {
        delete nextLevel;
    }
    nextLevel = NULL;
    pushLevel = NULL;
}

void bit::LevelManager::cascadeWindowEvent(sf::RenderWindow &window)
{
    if(currentLevel)
    {
        currentLevel->handleWindowChange(window);
    }
}

void bit::LevelManager::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(levels.size() > 0)
    {
        currentLevel->update(window, gameTime);

        // If there is a level to push
        if (pushLevel != NULL)
        {
            // Push level, level current alive
            levels.push_back(pushLevel);
            currentLevel = pushLevel;
            pushLevel = NULL;
        }

        // If we want to pop a level
        if (popLevel != NULL && currentLevel->currentStep == Level::LevelStateStep::Unloaded)
        {
            // Remove from stack
            delete levels.back();
            levels.pop_back();

            // Null refs
            popLevel = NULL;
            currentLevel = NULL;

            // If one was below, it becomes current
            if(levels.size() > 0)
            {
                currentLevel = levels.back();
            }
        }

        // If we want to change levels
        if (nextLevel != NULL && currentLevel->currentStep == Level::LevelStateStep::Unloaded)
        {
            // Remove from stack
            delete levels.back();
            levels.pop_back();
            currentLevel = NULL;

            // Push new level onto stack
            if (nextLevel != NULL)
            {
                levels.push_back(nextLevel);
                currentLevel = nextLevel;
                nextLevel = NULL;
            }
        }
    }
    else if (nextLevel != NULL)
    {
        // First level
        levels.push_back(nextLevel);
        currentLevel = nextLevel;
        nextLevel = NULL;
    }
}

void bit::LevelManager::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    if (currentLevel != NULL)
    {
        currentLevel->draw(window, gameTime);
    }
}

void bit::LevelManager::pop()
{
    if (currentLevel != NULL)
    {
        popLevel = currentLevel;
        currentLevel->unloadComponents();
    }
}

void bit::LevelManager::change(Level* newLevel)
{
    nextLevel = newLevel;
    if (currentLevel != NULL)
    {
        currentLevel->unloadComponents();
    }
}

void bit::LevelManager::push(Level* newLevel)
{
    pushLevel = newLevel;
}