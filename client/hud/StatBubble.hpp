#pragma once
#ifndef RZ_STATBUBBLE_H
#define RZ_STATBUBBLE_H

#include "SFML/Graphics.hpp"
#include "Frame.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class Hud;
class TileClient;

class StatBubble : public Frame
{
public:

    StatBubble(Hud* hud);

    bool isActive;
    unsigned int tileId;
    TileClient* tileClient;
    bit::GameTimer refreshTimer;
    bit::GameTimer fadeTimer;

    virtual void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

    void activate();

    void deactivate();

    virtual void hide();

    virtual void show();

    void handleStats(unsigned int tileId);
};

#endif