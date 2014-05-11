#include "Frame.hpp"
#include "Hud.hpp"
#include "../ClientRequest.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Network.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"
#include "../Interaction.hpp"
#include "../TileClient.hpp"

Frame::Frame(Hud* _hud)
    : bit::Container(0, 0, 100, 100, bit::Element::AnchorType::Center, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), hud(_hud), isActive(true)
{
}

void Frame::load(Hud* _hud)
{
    hud = _hud;

    topLeftQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    topRightQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    bottomLeftQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    bottomRightQuadIndex = hud->interfaceVertexMap.requestVertexIndex();

    cornerSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("hud_frame_corner");

    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[topLeftQuadIndex]);
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[topRightQuadIndex]);
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[bottomLeftQuadIndex]);
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[bottomRightQuadIndex]);
}

void Frame::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    // Set target dimensions
    Element::updateTargets(window, gameTime);

    // Update Target size
    //targetWidth = cornerSprite->width * 2;
    //targetHeight = cornerSprite->height * 2;
}

void Frame::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    // Update real position of label
    Element::updateReals(window, gameTime);

    // Top left corner
    cornerSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[topLeftQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[topLeftQuadIndex], left, top, 1, cornerSprite->width, cornerSprite->height);

    // Top right corner
    cornerSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[topRightQuadIndex]);
    bit::VertexHelper::flipQuad(&hud->interfaceVertexMap.vertexArray[topRightQuadIndex], true, false);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[topRightQuadIndex], left + width - cornerSprite->width, top, 1, cornerSprite->width, cornerSprite->height);

    // Bottom left corner
    cornerSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[bottomLeftQuadIndex]);
    bit::VertexHelper::flipQuad(&hud->interfaceVertexMap.vertexArray[bottomLeftQuadIndex], false, true);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[bottomLeftQuadIndex], left, top + height - cornerSprite->height, 1, cornerSprite->width, cornerSprite->height);
    
    // Bottom right corner
    cornerSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[bottomRightQuadIndex]);
    bit::VertexHelper::flipQuad(&hud->interfaceVertexMap.vertexArray[bottomRightQuadIndex], true, true);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[bottomRightQuadIndex], left + width - cornerSprite->width, top + height - cornerSprite->height, 1, cornerSprite->width, cornerSprite->height);
}

void Frame::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    Element::draw(window, gameTime);
}