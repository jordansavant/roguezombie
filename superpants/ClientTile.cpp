#include "ClientTile.hpp"
#include "Tile.hpp"
#include "World.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Math.hpp"

ClientTile::ClientTile()
    : Tile()
{
}

void ClientTile::clientLoad(sf::Texture* texture)
{
    renderTexture = texture;
    renderSprite.setTexture(*texture);
}

void ClientTile::clientUpdate(sf::Time &gameTime)
{
    renderSprite.setPosition(fixedState.x, fixedState.y);
}

void ClientTile::clientDraw(sf::RenderWindow &window, sf::Time &gameTime)
{
    window.draw(renderSprite);
}