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

void Zombie::load(World* _world, unsigned int _id, float _x, float _y)
{
    Character::load(_world, _id, Character::Type::Zombie, _x, _y, 1);
}

void Zombie::update(sf::Time &gameTime)
{
    Character::update(gameTime);

	if(!fixedState.isPlayerCharacter)
	{
		if(walkTimer.update(gameTime) && false)
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