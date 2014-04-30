#pragma once
#ifndef RZ_JOURNAL_H
#define RZ_JOURNAL_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"

class Hud;

class Journal : public bit::Container
{
public:

    Journal(Hud* hud);

    Hud* hud;
    bit::Label* journalEntries;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

};

#endif