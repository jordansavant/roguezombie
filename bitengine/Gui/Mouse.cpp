#include "Mouse.hpp"
#include <string>
#include "SFML/Graphics.hpp"
#include "../Game/VideoGame.hpp"

bit::Mouse::Mouse(std::string textureName)
    : color(255, 255, 255, 255)
{
    texture.loadFromFile(textureName);
    texture.setSmooth(true);
    sprite.setTexture(texture);
}

void bit::Mouse::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
    if(pixelPos.x < 0 || pixelPos.y < 0 || pixelPos.x > window.getSize().x || pixelPos.y > window.getSize().y)
    {
        window.setMouseCursorVisible(true);
    }
    else
    {
        window.setMouseCursorVisible(false);
        sprite.setPosition(pixelPos.x, pixelPos.y);
        sprite.setColor(color);
        window.draw(sprite);
    }
}