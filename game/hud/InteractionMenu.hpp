#pragma once
#ifndef RZ_INTERACTIONMENU_H
#define RZ_INTERACTIONMENU_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class Hud;

class InteractionMenu : public bit::Container
{
public:

    InteractionMenu(Hud* hud);

    Hud* hud;
    bool isActive;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void clear();

    void handleInteractionTree(bit::ServerPacket &packet, unsigned int tileId);
};

#endif