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

    Frame(Hud* hud, float relativeX, float relativeY, float width, float height, AnchorType anchorType);

    Frame(Hud* hud, float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput);

    Hud* hud;
    sf::Color color;
    
    unsigned int topLeftQuadIndex;
    unsigned int topRightQuadIndex;
    unsigned int bottomLeftQuadIndex;
    unsigned int bottomRightQuadIndex;
    bit::Sprite* cornerSprite;

    unsigned int topQuadIndex;
    unsigned int bottomQuadIndex;
    unsigned int leftQuadIndex;
    unsigned int rightQuadIndex;
    bit::Sprite* topEdgeSprite;
    bit::Sprite* bottomEdgeSprite;
    bit::Sprite* leftEdgeSprite;
    bit::Sprite* rightEdgeSprite;

    unsigned int backgroundQuadIndex;
    bit::Sprite* backgroundSprite;
    
    bool useBottomPointer;
    unsigned int bottomPointerQuadIndex;
    bit::Sprite* bottomPointerSprite;

    void load();

    virtual void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void updateReals(sf::RenderWindow &window, sf::Time &gameTime);
};

#endif