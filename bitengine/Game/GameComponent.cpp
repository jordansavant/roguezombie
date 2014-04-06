#include "GameComponent.hpp"
#include "Game.hpp"

bit::GameComponent::GameComponent(bit::Game* _game)
{
    game = _game;
}

bit::GameComponent::~GameComponent()
{
}

void bit::GameComponent::update(sf::Time &gameTime)
{
}

void bit::GameComponent::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
}