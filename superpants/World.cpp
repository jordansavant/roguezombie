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

void World::serverLoad()
{
    int zcount = 3;
    zombies.resize(zcount, NULL);
    for(unsigned int i=0; i < zcount; i++)
    {
        Zombie* z = new Zombie();
        z->serverLoad(i, this, 500 * bit::Math::randomFloat(), 500 * bit::Math::randomFloat());
        zombies[i] = z;
    }
}

void World::clientLoad()
{
    zombieimage.loadFromFile(resourcePath() + "Zombie.png");

    // NOT GOING TO STAY
    int zcount = 3;
    zombies.resize(zcount, NULL);
    for(unsigned int i=0; i < zcount; i++)
    {
        Zombie* z = new Zombie();
        z->clientLoad(&zombieimage);
        zombies[i] = z;
    }
}

void World::serverUpdate(sf::Time &gameTime)
{
    for(unsigned int i=0; i < zombies.size(); i++)
    {
        zombies[i]->serverUpdate(gameTime);
    }
}

void World::clientUpdate(sf::Time &gameTime)
{
    for(unsigned int i=0; i < zombies.size(); i++)
    {
        zombies[i]->clientUpdate(gameTime);
    }
}

void World::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    for(unsigned int i=0; i < zombies.size(); i++)
    {
        zombies[i]->draw(window, gameTime);
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

sf::Packet& World::extractSnapshot(sf::Packet &packet)
{
    sf::Uint32 zombieCount;
    packet >> zombieCount;

    for(unsigned int i=0; i < zombieCount; i++)
    {
        sf::Uint32 zombieId;
        packet >> zombieId;
        zombies[zombieId]->extractSnapshot(packet);
    }

    return packet;
}