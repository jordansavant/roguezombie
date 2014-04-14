#pragma once
#ifndef RZ_OPTIONSBAR_H
#define RZ_OPTIONSBAR_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"

class HudElement;
class StateGamePlay;

class OptionsBar : public bit::Container
{
public:

    OptionsBar(StateGamePlay* state);

    StateGamePlay* state;
    HudElement* options;
    HudElement* journal;
    HudElement* character;
    HudElement* inventory;
    HudElement* map;
    sf::RenderStates states;

    void draw(sf::RenderWindow &window, sf::Time &gameTime);
};

#endif