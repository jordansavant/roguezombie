#include "Frame.hpp"
#include "Hud.hpp"
#include "../../server/ClientRequest.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Network.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"
#include "../../server/Interaction.hpp"
#include "../TileClient.hpp"

Frame::Frame(Hud* _hud)
    : bit::Container(0, 0, 160, 200, bit::Element::AnchorType::Center, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), hud(_hud), useBottomPointer(false), color(sf::Color::White)
{
    load();
}

Frame::Frame(Hud* hud, float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : bit::Container(relativeX, relativeY, width, height, anchorType, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), hud(hud), useBottomPointer(false), color(sf::Color::White)
{
    load();
}

Frame::Frame(Hud* hud, float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput)
    : bit::Container(relativeX, relativeY, width, height, anchorType, lambdaListenToInput), hud(hud), useBottomPointer(false), color(sf::Color::White)
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
    topEdgeSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("hud_frame_top");
    bottomEdgeSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("hud_frame_bottom");
    leftEdgeSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("hud_frame_left");
    rightEdgeSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("hud_frame_right");
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

    color.a = 255 * opacity;
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[topLeftQuadIndex], color);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[topRightQuadIndex], color);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[bottomLeftQuadIndex], color);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[bottomRightQuadIndex], color);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[topQuadIndex], color);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[bottomQuadIndex], color);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[leftQuadIndex], color);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[rightQuadIndex], color);
    bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[backgroundQuadIndex], color);
    if(useBottomPointer)
    {
        bit::VertexHelper::colorQuad(&hud->interfaceVertexMap.vertexArray[bottomPointerQuadIndex], color);
    }
}

void Frame::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    // Update real position of label
    Container::updateReals(window, gameTime);

    int x, y, w, h;
    float z = Hud::zindex_frames;

    // Top left corner
    x = (int)left;
    y = (int)top;
    w = cornerSprite->width;
    h = cornerSprite->height;
    cornerSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[topLeftQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[topLeftQuadIndex], x, y, z, w, h);

    // Top right corner
    x = (int)left + (int)width - cornerSprite->width;
    y = (int)top;
    w = cornerSprite->width;
    h = cornerSprite->height;
    cornerSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[topRightQuadIndex]);
    bit::VertexHelper::flipQuad(&hud->interfaceVertexMap.vertexArray[topRightQuadIndex], true, false);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[topRightQuadIndex], x, y, z, w, h);

    // Bottom left corner
    x = (int)left;
    y = (int)top + (int)height - cornerSprite->height;
    w = cornerSprite->width;
    h = cornerSprite->height;
    cornerSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[bottomLeftQuadIndex]);
    bit::VertexHelper::flipQuad(&hud->interfaceVertexMap.vertexArray[bottomLeftQuadIndex], false, true);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[bottomLeftQuadIndex], x, y, z, w, h);
    
    // Bottom right corner
    x = (int)left + (int)width - cornerSprite->width;
    y = (int)top + (int)height - cornerSprite->height;
    w = cornerSprite->width;
    h = cornerSprite->height;
    cornerSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[bottomRightQuadIndex]);
    bit::VertexHelper::flipQuad(&hud->interfaceVertexMap.vertexArray[bottomRightQuadIndex], true, true);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[bottomRightQuadIndex], x, y, z, w, h);

    // Top
    x = (int)left + cornerSprite->width;
    y = (int)top;
    w = (int)width - cornerSprite->width * 2;
    h = topEdgeSprite->height;
    topEdgeSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[topQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[topQuadIndex], x, y, z, w, h);

    // Bottom
    x = (int)left + cornerSprite->width;
    y = (int)top + (int)height - cornerSprite->height;
    w = (int)width - cornerSprite->width * 2;
    h = bottomEdgeSprite->height;
    bottomEdgeSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[bottomQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[bottomQuadIndex], x, y, z, w, h);

    // Left
    x = (int)left;
    y = (int)top + cornerSprite->height;
    w = leftEdgeSprite->width;
    h = (int)height - cornerSprite->height * 2;
    leftEdgeSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[leftQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[leftQuadIndex], x, y, z, w, h);

    // Right
    w = (int)height - topEdgeSprite->height * 2;
    h = topEdgeSprite->width;
    x = (int)left + (int)width - topEdgeSprite->height / 2 - w / 2;
    y = (int)top + (int)height / 2 - h / 2;

    x = (int)left + (int)width - rightEdgeSprite->width;
    y = (int)top + (int)rightEdgeSprite->height;
    w = rightEdgeSprite->width;
    h = (int)height - cornerSprite->height * 2;
    rightEdgeSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[rightQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[rightQuadIndex], x, y, z, w, h);

    // Background
    x = (int)left + cornerSprite->width;
    y = (int)top + cornerSprite->height;
    w = (int)width - cornerSprite->width * 2;
    h = (int)height - cornerSprite->height * 2;
    backgroundSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[backgroundQuadIndex]);
    bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[backgroundQuadIndex], x, y, z, w, h);

    // Bottom Pointer
    if(useBottomPointer)
    {
        x = (int)left + (int)width / 2 - bottomPointerSprite->width / 2;
        y = (int)top + (int)height - 2;
        w = bottomPointerSprite->width;
        h = bottomPointerSprite->height;
        bottomPointerSprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[bottomPointerQuadIndex]);
        bit::VertexHelper::positionQuad(&hud->interfaceVertexMap.vertexArray[bottomPointerQuadIndex], x, y, z, w, h);
    }
}