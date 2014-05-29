#pragma once
#ifndef RZ_TOOLTIP_H
#define RZ_TOOLTIP_H

#include "SFML/Graphics.hpp"
#include "Frame.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class Hud;

class Tooltip : public Frame
{
public:

    Tooltip(Hud* hud);

    bool isActive;
    bit::GameTimer fadeTimer;
    bit::Label* information;

    virtual void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

    void activate();

    void deactivate();

    virtual void hide();

    virtual void show();

    void displayAt(std::string &info, int screenX, int screenY);
};

#endif