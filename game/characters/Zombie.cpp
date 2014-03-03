#include "Zombie.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/System.hpp"
#include "../World.hpp"
#include "../Tile.hpp"

Zombie::Zombie()
    : Character(), walkTimer(5)
{
}

void Zombie::load(World* _world, Tile* _tile)
{
    Character::load(_world, _tile);
}

void Zombie::update(sf::Time &gameTime)
{
    Character::update(gameTime);

	if(!isPlayerCharacter)
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