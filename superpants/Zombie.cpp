#include "Zombie.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Input/InputManager.hpp"
#include "../bitengine/Game/Game.hpp"
#include "../bitengine/Math/Math.hpp"
#include "MultiplayerState.hpp"

Zombie::Zombie()
    : x(0), y(0), direction(0, 0), health(100), maxHealth(100), isPlayer(false), walkTimer(2)
{
}

void Zombie::load(MultiplayerState* _state, sf::Texture &texture, float _x, float _y)
{
    state = _state;
    zombietexture = &texture;
    zombiesprite.setTexture(texture);
    x = _x;
    y = _y;
    centerX = x + zombietexture->getSize().x / 2;
    centerY = y + zombietexture->getSize().y / 2;
}

void Zombie::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(isPlayer)
    {
        direction.x = 0;
        direction.y = 0;
        if(state->game->inputManager->isButtonDown(sf::Keyboard::Up))
        {
            direction.y = -1;
        }
        if(state->game->inputManager->isButtonDown(sf::Keyboard::Down))
        {
            direction.y = 1;
        }
        if(state->game->inputManager->isButtonDown(sf::Keyboard::Right))
        {
            direction.x = 1;
        }
        if(state->game->inputManager->isButtonDown(sf::Keyboard::Left))
        {
            direction.x = -1;
        }
    }
    else
    {
        if(walkTimer.update(gameTime))
        {
            direction = bit::VectorMath::GetRandomVector();
        }
    }

    x += direction.x;
    y += direction.y;

    zombiesprite.setPosition(x, y);
}

void Zombie::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    window.draw(zombiesprite);
}