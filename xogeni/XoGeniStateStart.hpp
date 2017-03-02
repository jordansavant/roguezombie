#pragma once
#ifndef XG_XOGENISTATESTART_H
#define XG_XOGENISTATESTART_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Gui.hpp"

namespace XoGeni
{
    class XoGeniGame;

    class XoGeniStateStart : public bit::State
    {
    public:

        XoGeniStateStart(bit::StateStack &stack, XoGeniGame* game);

        virtual ~XoGeniStateStart();

        XoGeniGame* xoGeniGame;

        virtual bool update(sf::Time &gameTime);

        virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);
    };

}

#endif