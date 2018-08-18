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
    class LevelGenerator;
    class XoLevelRenderer;
    class CellMap;

    class XoGeniStateStart : public bit::State
    {
    public:

        XoGeniStateStart(bit::StateStack &stack, XoGeniGame* game);

        virtual ~XoGeniStateStart();
        
        bit::FrameTimer fps;
        XoGeniGame* xoGeniGame;
        XoLevelRenderer* levelRenderer;
        LevelGenerator* levelGenerator;
        CellMap* cellMap;
        unsigned int seedCounter;
        unsigned int difficultyCounter;

        virtual bool update(sf::Time &gameTime);

        virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

        virtual void drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera);
    };

}

#endif