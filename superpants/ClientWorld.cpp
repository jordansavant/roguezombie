#include "ClientWorld.hpp"
#include "ClientZombie.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/Network.hpp"
#include "../ResourcePath.h"
#include "SFML/Network.hpp"
#include <map>

ClientWorld::ClientWorld()
{
}

ClientWorld::~ClientWorld()
{
    for(unsigned int i=0; i < zombies.size(); i++)
    {
        delete zombies[i];
    }
}

void ClientWorld::load()
{
    zombieimage.loadFromFile(resourcePath() + "Zombie.png");
}

void ClientWorld::update(sf::Time &gameTime)
{
    for(auto iterator = zombies.begin(); iterator != zombies.end(); iterator++)
    {
        iterator->second->clientUpdate(gameTime);
    }
}

void ClientWorld::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    for(auto iterator = zombies.begin(); iterator != zombies.end(); iterator++)
    {
        iterator->second->clientDraw(window, gameTime);
    }
}

void ClientWorld::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    sf::Uint32 zombieCount;
    packet >> zombieCount;

    // Update all zombies
    for(unsigned int i=0; i < zombieCount; i++)
    {
        sf::Uint32 zombieId;
        packet >> zombieId;

        // If zombie exists, update it
        ClientZombie* z;
        auto itr = zombies.find(zombieId);
        if(itr != zombies.end())
        {
            z = itr->second;
        }
        // If not, create it, load it and update it
        else
        {
            z = new ClientZombie();
            z->clientLoad(&zombieimage);
            zombies.insert(std::pair<sf::Uint32, ClientZombie*>(zombieId, z));
        }
        z->handleSnapshot(packet, full);
    }

    // TODO
    // Remove any zombies that were not in snapshot
}