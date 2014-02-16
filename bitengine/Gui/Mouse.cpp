#include "Mouse.hpp"
#include <string>
#include "SFML/Graphics.hpp"
#include "../Game/Game.hpp"

bit::Mouse::Mouse(std::string textureName)
    : color(255, 255, 255, 255)
{
    texture.loadFromFile(textureName);
    texture.setSmooth(true);
    sprite.setTexture(texture);
}

void bit::Mouse::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    float scale = std::min((float)Game::currentResolution.x / (float)Game::targetResolution.x, (float)Game::currentResolution.y / (float)Game::targetResolution.y);
    window.setMouseCursorVisible(false);
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    sprite.setPosition(pixelPos.x, pixelPos.y);
    sprite.setScale(scale, scale);
    sprite.setColor(color);
    window.draw(sprite);
}