#include "ClientZombie.hpp"
#include "Zombie.hpp"
#include "World.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Input/InputManager.hpp"
#include "../bitengine/Game/Game.hpp"
#include "../bitengine/Math/Math.hpp"
#include "MultiplayerState.hpp"

ClientZombie::ClientZombie()
    : Zombie()
{
}

void ClientZombie::clientLoad(sf::Texture* texture)
{
    renderTexture = texture;
    renderSprite.setTexture(*texture);
}

void ClientZombie::clientUpdate(sf::Time &gameTime)
{
    renderSprite.setPosition(x, y);
}

void ClientZombie::clientDraw(sf::RenderWindow &window, sf::Time &gameTime)
{
    window.draw(renderSprite);
}