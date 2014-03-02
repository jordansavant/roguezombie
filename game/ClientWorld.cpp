#include "ClientWorld.hpp"
#include "ClientZombie.hpp"
#include "ClientTile.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/Network.hpp"
#include "../ResourcePath.h"
#include "SFML/Network.hpp"
#include <map>

ClientWorld::ClientWorld()
    : state(NULL)
{
}

ClientWorld::~ClientWorld()
{
    for(unsigned int i=0; i < tiles.size(); i++)
    {
        delete tiles[i];
    }
    for(unsigned int i=0; i < zombies.size(); i++)
    {
        delete zombies[i];
    }
}

void ClientWorld::load(GameplayState* _state)
{
    state = _state;
    zombieimage.loadFromFile(resourcePath() + "Zombie.png");
    tileimage.loadFromFile(resourcePath() + "water.png");
}

void ClientWorld::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    for(auto iterator = tiles.begin(); iterator != tiles.end(); iterator++)
    {
        iterator->second->clientUpdate(window, gameTime);
    }
    for(auto iterator = zombies.begin(); iterator != zombies.end(); iterator++)
    {
        iterator->second->clientUpdate(gameTime);
    }
}

void ClientWorld::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    for(auto iterator = tiles.begin(); iterator != tiles.end(); iterator++)
    {
        iterator->second->clientDraw(window, gameTime);
    }
    for(auto iterator = zombies.begin(); iterator != zombies.end(); iterator++)
    {
        iterator->second->clientDraw(window, gameTime);
    }
}

void ClientWorld::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    // Update all Tiles
    sf::Uint32 tileCount;
    packet >> tileCount;
    for(unsigned int i=0; i < tileCount; i++)
    {
        sf::Uint32 tileId;
        packet >> tileId;

        // If zombie exists, update it
        ClientTile* t;
        auto itr = tiles.find(tileId);
        if(itr != tiles.end())
        {
            t = itr->second;
        }
        // If not, create it, load it and update it
        else
        {
            t = new ClientTile();
            t->clientLoad(this, &tileimage);
            tiles.insert(std::pair<sf::Uint32, ClientTile*>(tileId, t));
        }
        t->handleSnapshot(packet, full);
    }

    // Update all zombies
    sf::Uint32 zombieCount;
    packet >> zombieCount;
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
            z->clientLoad(this, &zombieimage);
            zombies.insert(std::pair<sf::Uint32, ClientZombie*>(zombieId, z));
        }
        z->handleSnapshot(packet, full);
    }

}