#include "HudElement.hpp"
#include "Hud.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"

HudElement::HudElement(float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : Element(relativeX, relativeY, width, height, anchorType), isActive(false)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    canHaveFocus = true;
}

HudElement::HudElement(float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput)
    : Element(relativeX, relativeY, width, height, anchorType, lambdaListenToInput), isActive(false)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    canHaveFocus = true;
}

void HudElement::load(Hud* _hud, std::string &spritename)
{
    hud = _hud;

    quadIndex = hud->interfaceVertexMap.requestVertexIndex();
    sprite = hud->state->rogueZombieGame->spriteLoader->getSprite(spritename);
    sprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[quadIndex]);
}

void HudElement::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    // Set target dimensions
    Element::updateTargets(window, gameTime);

    // Update Target size
    targetWidth = sprite->width;
    targetHeight = sprite->height;
}

void HudElement::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    // Update real position of label
    Element::updateReals(window, gameTime);

    sf::Color color(isInfocus ? sf::Color(200, 0, 0) : sf::Color::White);

    // Set Sprites details
    sprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[quadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[quadIndex], left, top, 1, width, height);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[quadIndex], color);
}

void HudElement::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    Element::draw(window, gameTime);
}