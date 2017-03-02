#pragma once
#ifndef XG_XOGENIGAME_H
#define XG_XOGENIGAME_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Gui.hpp"
#include <string>

namespace XoGeni
{
    class XoGeniGame: public bit::VideoGame
    {
    public:
            XoGeniGame();

            static unsigned int stateStart;

            bit::Mouse mouse;

            virtual void update(sf::Time &gameTime);

            virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

            virtual void registerStates();
    };
}

#endif