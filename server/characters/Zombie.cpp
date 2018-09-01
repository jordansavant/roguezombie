#include "Zombie.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/System.hpp"
#include "../Level.hpp"
#include "../Tile.hpp"

Zombie::Zombie()
    : Character(), walkTimer(5)
{
    consumptionHeal = 0; // if eaten I will not heal
}

void Zombie::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Character::load(_level, _id, Character::Type::Zombie, _x, _y, _level->tileWidth * 1, _level->tileHeight * 1);
}