#include "TileClient.hpp"
#include "Tile.hpp"
#include "WorldClient.hpp"
#include "GameplayState.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Math.hpp"
#include <sstream>

TileClient::TileClient()
    : Tile()
{
}

void TileClient::clientLoad(WorldClient* _world)
{
    world = _world;

    quadIndex = world->vertexMap_01.requestVertexIndex();
    sprite = world->state->game->spriteLoader->getSprite("Water");
    sprite->applyToQuad(&world->vertexMap_01.vertexArray[quadIndex]);
}

void TileClient::clientUpdate(sf::RenderWindow &window, sf::Time &gameTime)
{
    sf::Vector2f isoPosition = bit::VectorMath::normalToIsometric(fixedState.x, fixedState.y);
    bit::Vertex3* quad = &world->vertexMap_01.vertexArray[quadIndex];
    bit::VertexHelper::positionQuad(quad, isoPosition.x - fixedState.width, isoPosition.y, 0, 64, 32);

    // Test Mouse translation
    sf::Vector2f mouseWorldIsoPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    sf::Vector2f mouseWorldPosition = bit::VectorMath::isometricToNormal(mouseWorldIsoPos.x, mouseWorldIsoPos.y);

    float mx = std::floor((float)mouseWorldPosition.x / (float)fixedState.width);
    float my = std::floor((float)mouseWorldPosition.y / (float)fixedState.height);
    float tx = std::floor((float)fixedState.x / (float)fixedState.width);
    float ty = std::floor((float)fixedState.y / (float)fixedState.height);

    if(tx == mx && ty == my)
    {
        sf::Color red(255, 0, 0);
        bit::VertexHelper::colorQuad(quad, red);

        // Set me as being hovered over
        world->hoveredTile = this;
    }
    else
    {
        sf::Color white(255, 255, 255);
        bit::VertexHelper::colorQuad(quad, white);

        // Unset if I was previously set
        if(world->hoveredTile == this)
        {
            world->hoveredTile = NULL;
        }
    }
}