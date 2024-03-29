#include "Scientist.hpp"
#include "../Level.hpp"
#include "../GameplayServer.hpp"
#include "SFML/Graphics.hpp"

Scientist::Scientist()
    : Character()
{
    consumptionHeal = 10; // if eaten I will heal
}

void Scientist::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Character::load(_level, _id, Character::Type::Scientist, _x, _y, _level->tileWidth, _level->tileHeight);

    schema.maxHealth = 5;
    schema.health = 5;
    schema.speed = 2;
}

void Scientist::update(sf::Time &gameTime)
{
    Character::update(gameTime);
}