#include "TileClient.hpp"
#include "Tile.hpp"
#include "WorldClient.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Math.hpp"
#include <sstream>

TileClient::TileClient()
    : Tile()
{
}

void TileClient::clientLoad(WorldClient* _world, sf::Texture* texture, sf::Font* _font)
{
    world = _world;
    renderTexture = texture;
    renderSprite.setTexture(*texture);
    font = _font;
    text.setFont(*font);
    text.setCharacterSize(12);
}

void TileClient::clientUpdate(sf::RenderWindow &window, sf::Time &gameTime)
{
    std::stringstream ss;
    ss << fixedState.ID;
    text.setString(ss.str());


    sf::Vector2f isoPosition = bit::VectorMath::normalToIsometric(fixedState.x, fixedState.y);
    renderSprite.setPosition(isoPosition.x - fixedState.width, isoPosition.y);

    text.setPosition(isoPosition.x, isoPosition.y);

    // Test Mouse translation
    sf::Vector2f mouseWorldIsoPos = window.mapPixelToCoords(sf::Mouse::getPosition(window));
    sf::Vector2f mouseWorldPosition = bit::VectorMath::isometricToNormal(mouseWorldIsoPos.x, mouseWorldIsoPos.y);

    float mx = std::floor((float)mouseWorldPosition.x / (float)fixedState.width);
    float my = std::floor((float)mouseWorldPosition.y / (float)fixedState.height);
    float tx = std::floor((float)fixedState.x / (float)fixedState.width);
    float ty = std::floor((float)fixedState.y / (float)fixedState.height);

    if(tx == mx && ty == my)
    {
        
        renderSprite.setColor(sf::Color::Red);
    }
    else
    {
        renderSprite.setColor(sf::Color::White);
    }
}

void TileClient::clientDraw(sf::RenderWindow &window, sf::Time &gameTime)
{
    window.draw(renderSprite);
    window.draw(text);
}