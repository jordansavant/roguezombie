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
    : bit::Container(0, 0, 160, 200, bit::Element::AnchorType::Center, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), hud(_hud), isActive(true)
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

    topQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    bottomQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    leftQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    rightQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    edgeSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("hud_frame_top");
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[topQuadIndex]);
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[bottomQuadIndex]);
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[leftQuadIndex]);
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[rightQuadIndex]);
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

    // Top
    edgeSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[topQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[topQuadIndex], left + cornerSprite->width - 1, top, 1, width - cornerSprite->width * 2 + 2, edgeSprite->height);

    // Bottom
    edgeSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[bottomQuadIndex]);
    bit::VertexHelper::flipQuad(&hud->interfaceVertexMap.vertexArray[bottomQuadIndex], false, true);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[bottomQuadIndex], left + cornerSprite->width - 1, top + height - cornerSprite->height, 1, width - cornerSprite->width * 2 + 2, edgeSprite->height);

    // Left
    float leftWidth = height - edgeSprite->height * 2 + 2;
    float leftHeight = edgeSprite->width;
    float leftX = left - leftWidth / 2 + edgeSprite->height / 2;
    float leftY = top + height / 2 - leftHeight / 2;
    edgeSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[leftQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[leftQuadIndex], leftX, leftY, 1, leftWidth, leftHeight);
    bit::VertexHelper::rotateQuad(&hud->interfaceVertexMap.vertexArray[leftQuadIndex], -bit::Math::PiOver2);

    // Right
    float rightWidth = height - edgeSprite->height * 2 + 2;
    float rightHeight = edgeSprite->width;
    float rightX = left + width - edgeSprite->height / 2 - rightWidth / 2;
    float rightY = top + height / 2 - rightHeight / 2;
    edgeSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[rightQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[rightQuadIndex], rightX, rightY, 1, rightWidth, rightHeight);
    bit::VertexHelper::rotateQuad(&hud->interfaceVertexMap.vertexArray[rightQuadIndex], bit::Math::PiOver2);
}

void Frame::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    Element::draw(window, gameTime);
}