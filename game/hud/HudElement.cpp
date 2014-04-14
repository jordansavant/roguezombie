#include "HudElement.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"

HudElement::HudElement(float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : Element(relativeX, relativeY, width, height, anchorType), isActive(false)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
}

HudElement::HudElement(float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput)
    : Element(relativeX, relativeY, width, height, anchorType, lambdaListenToInput), isActive(false)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
}

void HudElement::load(StateGamePlay* _state, std::string &spritename)
{
    state = _state;

    quadIndex = state->interfaceVertexMap.requestVertexIndex();
    sprite = state->rogueZombieGame->spriteLoader->getSprite(spritename);
    sprite->applyToQuad(&state->interfaceVertexMap.vertexArray[quadIndex]);
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

    // Set Sprites details
    sprite->applyToQuad(&state->interfaceVertexMap.vertexArray[quadIndex]);
    bit::VertexHelper::positionQuad(&state->interfaceVertexMap.vertexArray[quadIndex], left, top, 1, width, height);
}

void HudElement::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    Element::draw(window, gameTime);
}