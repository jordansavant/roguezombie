#include "WallClient.hpp"
#include "Wall.hpp"
#include "SFML/Graphics.hpp"
#include "../WorldClient.hpp"
#include "../GameplayState.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/System.hpp"

WallClient::WallClient()
    : Wall()
{
}

void WallClient::clientLoad(WorldClient* _world, sf::Texture* texture)
{
    world = _world;
    renderTexture = texture;
    renderSprite.setTexture(*texture);
}

void WallClient::clientUpdate(sf::Time &gameTime)
{
    float spriteWidth = 64;
    float spriteHeight = 64;
    float xFootOffset = 0;
    float yFootOffset = 16;

    float worldX = Body::deltaState.x;
    float worldY = Body::deltaState.y;
    float worldCenterX = worldX + Body::deltaState.width / 2;
    float worldCenterY = worldY + Body::deltaState.height / 2;

    sf::Vector2f renderPosition = bit::VectorMath::normalToIsometric(worldCenterX, worldCenterY);
    float renderX = renderPosition.x - spriteWidth / 2 + xFootOffset / 2;
    float renderY = renderPosition.y - spriteHeight + yFootOffset;
    renderSprite.setPosition(renderX, renderY);
}

void WallClient::clientDraw(sf::RenderWindow &window, sf::Time &gameTime)
{
    window.draw(renderSprite);
}