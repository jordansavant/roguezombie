#include "Ogre.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/System.hpp"
#include "../Level.hpp"
#include "../Tile.hpp"

Ogre::Ogre()
    : Character(), walkTimer(5)
{
}

void Ogre::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Character::load(_level, _id, Character::Type::Ogre, _x, _y, _level->tileWidth * 2, _level->tileHeight * 2);
}

void Ogre::update(sf::Time &gameTime)
{
    Character::update(gameTime);

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