#include "TileClient.hpp"
#include "Tile.hpp"
#include "LevelClient.hpp"
#include "ClientGameplayState.hpp"
#include "RogueZombieGame.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Math.hpp"
#include <sstream>

TileClient::TileClient()
    : Tile(), lastSnapshotId(0)
{
}

TileClient::~TileClient()
{
}

void TileClient::clientLoad(LevelClient* _level)
{
    level = _level;

    // Game quad
    quadIndex = level->vertexMap_01.requestVertexIndex();
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite("Water");
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}

void TileClient::clientUpdate(sf::Time &gameTime)
{
    // Sprite
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);

    // Position
    sf::Vector2f isoPosition = bit::VectorMath::normalToIsometric(fixedState.x, fixedState.y);
    bit::Vertex3* quad = &level->vertexMap_01.vertexArray[quadIndex];
    bit::VertexHelper::positionQuad(quad, isoPosition.x - fixedState.width, isoPosition.y, 0, 64, 32);

    // Test Mouse translation
    sf::Vector2f mouseLevelIsoPos = level->state->rogueZombieGame->renderWindow->mapPixelToCoords(sf::Mouse::getPosition(*level->state->rogueZombieGame->renderWindow));
    sf::Vector2f mouseLevelPosition = bit::VectorMath::isometricToNormal(mouseLevelIsoPos.x, mouseLevelIsoPos.y);

    float mx = std::floor((float)mouseLevelPosition.x / (float)fixedState.width);
    float my = std::floor((float)mouseLevelPosition.y / (float)fixedState.height);
    float tx = std::floor((float)fixedState.x / (float)fixedState.width);
    float ty = std::floor((float)fixedState.y / (float)fixedState.height);

    if(tx == mx && ty == my)
    {
        sf::Color red(255, 0, 0);
        bit::VertexHelper::colorQuad(quad, red);

        // Set me as being hovered over
        level->hoveredTile = this;
    }
    else
    {
        int r = deltaState.rshade * deltaState.illumination;
        int g = deltaState.gshade * deltaState.illumination;
        int b = deltaState.bshade * deltaState.illumination;
        sf::Color c(r, g, b);
        bit::VertexHelper::colorQuad(quad, c);

        // Unset if I was previously set
        if(level->hoveredTile == this)
        {
            level->hoveredTile = NULL;
        }
    }
}

void TileClient::reset()
{
    bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}
