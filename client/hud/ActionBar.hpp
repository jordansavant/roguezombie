#pragma once
#ifndef RZ_ACTIONBAR_H
#define RZ_ACTIONBAR_H

#include "Frame.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"
#include "../items/ItemClient.hpp"

class Hud;
class HudElement;
class ActionBarSlot;

class ActionBar : public Frame
{
public:

    ActionBar(Hud* hud);

    float originX, originY;
    HudElement* wait;
    HudElement* attack;
    HudElement* swap;
    std::vector<ActionBarSlot*> slots;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void hide();

    virtual void show();
};

#endif