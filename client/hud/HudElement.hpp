#pragma once
#ifndef RZ_HUDELEMENT_H
#define RZ_HUDELEMENT_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"

class Hud;

class HudElement : public bit::Element
{
public:
    HudElement(float relativeX, float relativeY, float width, float height, AnchorType anchorType);

    HudElement(float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput);

    bool isActive;

    unsigned int quadIndex;
    Hud* hud;
    bit::Sprite* sprite;
    sf::Color color;

    void load(Hud* hud, std::string &spritename);

    void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

    void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

    void draw(sf::RenderWindow &window, sf::Time &gameTime);

    void hide(bit::Effect* effect = NULL);

    void show(bit::Effect* effect = NULL);
};

#endif