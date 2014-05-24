#include "MoveMarker.hpp"
#include "LevelClient.hpp"
#include "StateGamePlay.hpp"
#include "RogueZombieGame.hpp"

MoveMarker::MoveMarker()
    : level(NULL), sprite(NULL), quadIndex(0), renderX(0), renderY(0)
{
}

void MoveMarker::load(LevelClient* _level)
{
    level = _level;

    // Game quad
    quadIndex = level->vertexMap_01.requestVertexIndex();
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite("MoveMarker");
    bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}

void MoveMarker::renderAt(int worldX, int worldY, sf::Color &color)
{
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);
    sf::Vector2f isoPosition = bit::VectorMath::normalToIsometric(worldX, worldY);
    renderX = isoPosition.x - 16;//schema.width;
    renderY = isoPosition.y + 8;
    bit::VertexHelper::positionQuad(&level->vertexMap_01.vertexArray[quadIndex], renderX, renderY, 1, sprite->width, sprite->height);
    bit::VertexHelper::colorQuad(&level->vertexMap_01.vertexArray[quadIndex], color);
}

void MoveMarker::hide()
{
    bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}