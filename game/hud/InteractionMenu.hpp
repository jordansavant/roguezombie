#pragma once
#ifndef RZ_INTERACTIONMENU_H
#define RZ_INTERACTIONMENU_H

#include "SFML/Graphics.hpp"
#include "Frame.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class Hud;

class InteractionMenu : public Frame
{
public:

    InteractionMenu(Hud* hud);

    bool isActive;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void deactivate();

    void handleInteractionTree(bit::ServerPacket &packet, unsigned int tileId);
};

#endif