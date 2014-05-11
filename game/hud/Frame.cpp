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
    : bit::Container(0, 0, 160, 200, bit::Element::AnchorType::Center, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), hud(_hud), useBottomPointer(false)
{
    load();
}

Frame::Frame(Hud* hud, float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : bit::Container(relativeX, relativeY, width, height, anchorType, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), hud(hud), useBottomPointer(false)
{
    load();
}

Frame::Frame(Hud* hud, float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput)
    : bit::Container(relativeX, relativeY, width, height, anchorType, lambdaListenToInput), hud(hud), useBottomPointer(false)
{
    load();
}

void Frame::load()
{
    opacity = 0;

    // Corners
    topLeftQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    topRightQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    bottomLeftQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    bottomRightQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    cornerSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("hud_frame_corner");
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[topLeftQuadIndex]);
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[topRightQuadIndex]);
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[bottomLeftQuadIndex]);
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[bottomRightQuadIndex]);

    // Sides
    topQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    bottomQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    leftQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    rightQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    edgeSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("hud_frame_top");
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[topQuadIndex]);
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[bottomQuadIndex]);
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[leftQuadIndex]);
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[rightQuadIndex]);
    
    // Background
    backgroundQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    backgroundSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("hud_frame_background");
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[backgroundQuadIndex]);

    // Bottom Pointer
    bottomPointerQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
    bottomPointerSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("hud_frame_bottompointer");
    bit::VertexHelper::resetQuad(&hud->interfaceVertexMap.vertexArray[bottomPointerQuadIndex]);
}

void Frame::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    // Set target dimensions
    Container::updateTargets(window, gameTime);

    // Update Target size
    //targetWidth = cornerSprite->width * 2;
    //targetHeight = cornerSprite->height * 2;
}

void Frame::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    // Update real position of label
    Container::updateReals(window, gameTime);

    sf::Color color(255, 255, 255, 255 * opacity);

    // Top left corner
    cornerSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[topLeftQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[topLeftQuadIndex], left, top, 1, cornerSprite->width, cornerSprite->height);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[topLeftQuadIndex], color);

    // Top right corner
    cornerSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[topRightQuadIndex]);
    bit::VertexHelper::flipQuad(&hud->interfaceVertexMap.vertexArray[topRightQuadIndex], true, false);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[topRightQuadIndex], left + width - cornerSprite->width, top, 1, cornerSprite->width, cornerSprite->height);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[topRightQuadIndex], color);

    // Bottom left corner
    cornerSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[bottomLeftQuadIndex]);
    bit::VertexHelper::flipQuad(&hud->interfaceVertexMap.vertexArray[bottomLeftQuadIndex], false, true);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[bottomLeftQuadIndex], left, top + height - cornerSprite->height, 1, cornerSprite->width, cornerSprite->height);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[bottomLeftQuadIndex], color);
    
    // Bottom right corner
    cornerSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[bottomRightQuadIndex]);
    bit::VertexHelper::flipQuad(&hud->interfaceVertexMap.vertexArray[bottomRightQuadIndex], true, true);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[bottomRightQuadIndex], left + width - cornerSprite->width, top + height - cornerSprite->height, 1, cornerSprite->width, cornerSprite->height);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[bottomRightQuadIndex], color);

    // Top
    edgeSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[topQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[topQuadIndex], left + cornerSprite->width - 1, top, 1, width - cornerSprite->width * 2 + 2, edgeSprite->height);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[topQuadIndex], color);

    // Bottom
    edgeSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[bottomQuadIndex]);
    bit::VertexHelper::flipQuad(&hud->interfaceVertexMap.vertexArray[bottomQuadIndex], false, true);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[bottomQuadIndex], left + cornerSprite->width - 1, top + height - cornerSprite->height, 1, width - cornerSprite->width * 2 + 2, edgeSprite->height);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[bottomQuadIndex], color);

    // Left
    float leftWidth = height - edgeSprite->height * 2 + 2;
    float leftHeight = edgeSprite->width;
    float leftX = left - leftWidth / 2 + edgeSprite->height / 2;
    float leftY = top + height / 2 - leftHeight / 2;
    edgeSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[leftQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[leftQuadIndex], leftX, leftY, 1, leftWidth, leftHeight);
    bit::VertexHelper::rotateQuad(&hud->interfaceVertexMap.vertexArray[leftQuadIndex], -bit::Math::PiOver2);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[leftQuadIndex], color);

    // Right
    float rightWidth = height - edgeSprite->height * 2 + 2;
    float rightHeight = edgeSprite->width;
    float rightX = left + width - edgeSprite->height / 2 - rightWidth / 2;
    float rightY = top + height / 2 - rightHeight / 2;
    edgeSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[rightQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[rightQuadIndex], rightX, rightY, 1, rightWidth, rightHeight);
    bit::VertexHelper::rotateQuad(&hud->interfaceVertexMap.vertexArray[rightQuadIndex], bit::Math::PiOver2);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[rightQuadIndex], color);

    // Background
    backgroundSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[backgroundQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[backgroundQuadIndex], left + cornerSprite->width - 1, top + cornerSprite->height - 1, 1, width - cornerSprite->width * 2 + 2, height - cornerSprite->height * 2 + 2);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[backgroundQuadIndex], color);

    // Bottom Pointer
    if(useBottomPointer)
    {
        bottomPointerSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[bottomPointerQuadIndex]);
        bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[bottomPointerQuadIndex], left + width / 2 - bottomPointerSprite->width / 2, top + height - 2, 1, bottomPointerSprite->width, bottomPointerSprite->height);
        bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[bottomPointerQuadIndex], color);
    }
}