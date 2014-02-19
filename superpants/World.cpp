#include "World.hpp"
#include "Zombie.hpp"
#include "../bitengine/Math/Math.hpp"
#include "../ResourcePath.h"

World::World()
{
}

World::~World()
{
    for(unsigned int i=0; i < zombies.size(); i++)
    {
        delete zombies[i];
    }
}

void World::load(MultiplayerState* _state)
{
    state = _state;
    zombieimage.loadFromFile(resourcePath() + "Zombie.png");

    int zcount = 3;
    zombies.resize(zcount, NULL);
    for(unsigned int i=0; i < zcount; i++)
    {
        Zombie* z = new Zombie();
        z->load(state, zombieimage, 500 * bit::Math::randomFloat(), 500 * bit::Math::randomFloat());
        zombies[i] = z;

        if(i == 0)
        {
            z->isPlayer = true;
        }
    }
}


void World::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    for(unsigned int i=0; i < zombies.size(); i++)
    {
        zombies[i]->update(window, gameTime);
    }
}


void World::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    for(unsigned int i=0; i < zombies.size(); i++)
    {
        zombies[i]->draw(window, gameTime);
    }
}