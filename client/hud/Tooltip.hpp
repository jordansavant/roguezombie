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
    float requestedOpacity;
    int requestedDelay;
    int requestedDuration;
    std::string tag;

    virtual void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

    void activate();

    void deactivate();

    virtual void hide();

    virtual void show();

    void displayAt(std::string const&info, std::string const&tag, int screenX, int screenY, float opacity = 1, int delay = 750, int duration = 3000, int width = -1, int height = -1);
};

#endif