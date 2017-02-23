#pragma once
#ifndef RZ_DIALOGMENU_H
#define RZ_DIALOGMENU_H

#include "SFML/Graphics.hpp"
#include "Frame.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class Hud;
class TileClient;

class DialogMenu : public Frame
{
public:

    DialogMenu(Hud* hud);

    bool isActive;
    unsigned int tileId;
    TileClient* tileClient;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

    void activate();

    void deactivate();

    virtual void hide();

    virtual void show();

    void handleDialogPacket(bit::ServerPacket &packet, unsigned int tileId);
};

#endif