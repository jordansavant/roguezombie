#pragma once
#ifndef RZ_OPTIONS_H
#define RZ_OPTIONS_H

#include "HudMenu.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"

class Hud;

class Options : public HudMenu
{
public:

    Options(Hud* hud);

    unsigned int naviconQuadIndex;
    bit::Sprite* naviconSprite;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

    void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void hide();

    virtual void show();

};

#endif