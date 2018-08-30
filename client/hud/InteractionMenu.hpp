#pragma once
#ifndef RZ_INTERACTIONMENU_H
#define RZ_INTERACTIONMENU_H

#include "SFML/Graphics.hpp"
#include "Frame.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class Hud;
class TileClient;

class InteractionMenu : public Frame
{
public:

    InteractionMenu(Hud* hud);

    bool isActive;
    unsigned int tileId;
    TileClient* tileClient;
    bool hasInteractionUnderway;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

    void activate();

    void deactivate();

    virtual void hide();

    virtual void show();

    void handleNumberInput(unsigned int number);

    void handleInteractionTree(bit::ServerPacket &packet, unsigned int tileId);
};

#endif