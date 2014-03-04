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
    sf::Vector2f renderPosition = bit::VectorMath::normalToIsometric(Body::deltaState.x, Body::deltaState.y);
    renderPosition.x -= renderSprite.getLocalBounds().width / 2;
    renderPosition.y -= renderSprite.getLocalBounds().height - 16;
    renderSprite.setPosition(renderPosition.x, renderPosition.y);
}

void WallClient::clientDraw(sf::RenderWindow &window, sf::Time &gameTime)
{
    window.draw(renderSprite);
}
