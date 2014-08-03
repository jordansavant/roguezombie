#pragma once
#ifndef RZ_HUDMENU_H
#define RZ_HUDMENU_H

#include "Frame.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"

class Hud;

class HudMenu : public Frame
{
public:

    HudMenu(Hud* hud);

    HudMenu(Hud* hud, float relativeX, float relativeY, float width, float height, AnchorType anchorType);

    HudMenu(Hud* hud, float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput);

    bool isShown;
    float originX, originY;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void hide();

    virtual void show();

    virtual void toggle();

};

#endif