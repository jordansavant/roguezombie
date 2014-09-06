#include "MoveMarker.hpp"
#include "LevelClient.hpp"
#include "StateGamePlay.hpp"
#include "RogueZombieGame.hpp"
#include "RZConfig.hpp"

MoveMarker::MoveMarker()
    : level(NULL), sprite(NULL), quadIndex(0), renderX(0), renderY(0)
{
}

void MoveMarker::load(LevelClient* _level)
{
    level = _level;

    // Game quad
    map = &level->vertexMap_charactersConstIlluminated;
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite("MoveMarker");
    quadIndex = map->requestVertexIndex();
    bit::VertexHelper::resetQuad(&map->vertexArray[quadIndex]);
}

void MoveMarker::renderAt(int worldX, int worldY, sf::Color &color)
{
    sprite->applyToQuad(&map->vertexArray[quadIndex]);
    sf::Vector2f isoPosition = bit::VectorMath::normalToIsometric(worldX, worldY);
    renderX = isoPosition.x - sprite->width / 2;
    renderY = isoPosition.y + sprite->height / 2;
    float z = RZConfig::getDrawDepthForGameplay(renderY);
    bit::VertexHelper::positionQuad(&map->vertexArray[quadIndex], renderX, renderY, z, sprite->width, sprite->height);
    bit::VertexHelper::colorQuad(&map->vertexArray[quadIndex], color);
}

void MoveMarker::hide()
{
    bit::VertexHelper::resetQuad(&map->vertexArray[quadIndex]);
}