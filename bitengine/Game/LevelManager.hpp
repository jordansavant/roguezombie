#pragma once
#ifndef BIT_LEVELMANAGER_H
#define BIT_LEVELMANAGER_H

#include <vector>
#include "GameComponent.hpp"
#include "Level.hpp"

namespace bit
{
	class LevelManager : public GameComponent
	{
    public:
		LevelManager();

        virtual ~LevelManager();

        Level* currentLevel;

        virtual void cascadeWindowEvent(sf::RenderWindow &window);

		virtual void update(sf::RenderWindow &window, sf::Time &gameTime, bool isGamePaused);

		virtual void draw(sf::RenderWindow &window, sf::Time &gameTime, bool isGamePaused);

        virtual void pop();

        virtual void change(Level* newLevel);

        virtual void push(Level* newLevel);

    protected:
        std::vector<Level*> levels;
        Level* nextLevel;
        Level* pushLevel;
        Level* popLevel;
    };
}

#endif