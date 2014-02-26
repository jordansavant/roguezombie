#include "World.hpp"
#include "Zombie.hpp"
#include "../bitengine/Math/Math.hpp"
#include "../ResourcePath.h"
#include "SFML/Network.hpp"

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

void World::load()
{
    int zcount = 3;
    zombies.resize(zcount, NULL);
    for(unsigned int i=0; i < zcount; i++)
    {
        Zombie* z = new Zombie();
        z->load(this, 500 * bit::Math::randomFloat(), 500 * bit::Math::randomFloat());
        zombies[i] = z;
    }
}

void World::update(sf::Time &gameTime)
{
    for(unsigned int i=0; i < zombies.size(); i++)
    {
        zombies[i]->update(gameTime);
    }
}

sf::Packet& World::compileSnapshot(sf::Packet &packet)
{
    sf::Uint32 zombieCount = zombies.size();
    packet << zombieCount;

    for(unsigned int i=0; i < zombies.size(); i++)
    {
        sf::Uint32 zombieId = i;
        packet << zombieId;
        zombies[i]->compileSnapshot(packet);
    }

    return packet;
}
