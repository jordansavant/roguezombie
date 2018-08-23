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

void Zombie::updateAlive_Free(sf::Time &gameTime)
{
    Character::updateAlive_Free(gameTime);

    if(!schema.isPlayerCharacter)
    {
        if(walkTimer.update(gameTime))
        {
            walkTimer.adjustBy(2 * bit::Math::randomFloat());

            switch(bit::Math::random(4))
            {
                case 0:
                    moveUp();
                    break;
                case 1:
                    moveDown();
                    break;
                case 2:
                    moveLeft();
                    break;
                case 3:
                    moveRight();
                    break;
            }
        }
    }
}