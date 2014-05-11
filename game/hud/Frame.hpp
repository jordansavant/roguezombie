#pragma once
#ifndef RZ_FRAME_H
#define RZ_FRAME_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class Hud;

class Frame : public bit::Container
{
public:

    Frame(Hud* hud);

    Hud* hud;
    bool isActive;
    
    unsigned int topLeftQuadIndex;
    unsigned int topRightQuadIndex;
    unsigned int bottomLeftQuadIndex;
    unsigned int bottomRightQuadIndex;
    bit::Sprite* cornerSprite;

    unsigned int topQuadIndex;
    unsigned int bottomQuadIndex;
    unsigned int leftQuadIndex;
    unsigned int rightQuadIndex;
    bit::Sprite* edgeSprite;

    void load(Hud* hud);

    void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

    void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

	void draw(sf::RenderWindow &window, sf::Time &gameTime);
};

#endif