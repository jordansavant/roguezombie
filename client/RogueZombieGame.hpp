#pragma once
#ifndef RZ_ROGUEZOMBIEGAME_H
#define RZ_ROGUEZOMBIEGAME_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Gui.hpp"
#include <string>

class RogueZombieGame: public bit::VideoGame
{
public:
        RogueZombieGame();

        static unsigned int stateGameIntroduction;
        static unsigned int stateGameStart;
        static unsigned int stateGamePlayHost;
        static unsigned int stateGamePlayClient;
        static unsigned int stateGamePlayLocal;
        static unsigned int stateGameError;

        std::string errorMessage;
        bit::Mouse mouse;
        unsigned int exploreMusic;
        unsigned int combatMusic;

        virtual void update(sf::Time &gameTime);

        virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void registerStates();
};

#endif