#pragma once
#ifndef RZ_JOURNAL_H
#define RZ_JOURNAL_H

#include "HudMenu.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"

class Hud;

class Journal : public HudMenu
{
public:

    Journal(Hud* hud);

    bit::Label* journalEntries;
    unsigned int naviconQuadIndex;
    bit::Sprite* naviconSprite;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

    void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void hide();

    virtual void show();

};

#endif