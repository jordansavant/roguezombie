#include "WorldClient.hpp"
#include "TileClient.hpp"
#include "GameplayState.hpp"
#include "characters/ZombieClient.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/Network.hpp"
#include "../ResourcePath.h"
#include "SFML/Network.hpp"
#include <map>

WorldClient::WorldClient()
    : state(NULL), playerCharacter(NULL)
{
}

WorldClient::~WorldClient()
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

void WorldClient::load(GameplayState* _state)
{
    state = _state;
    zombieimage.loadFromFile(resourcePath() + "Zombie.png");
    tileimage.loadFromFile(resourcePath() + "Water.png");
    font.loadFromFile(resourcePath() + "Agency.ttf");
}

void WorldClient::update(sf::RenderWindow &window, sf::Time &gameTime)
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

void WorldClient::draw(sf::RenderWindow &window, sf::Time &gameTime)
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

void WorldClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    // Update all Tiles
    sf::Uint32 tileCount;
    packet >> tileCount;
    for(unsigned int i=0; i < tileCount; i++)
    {
        unsigned int tileId;
        packet >> tileId;

        // If tile exists, update it
        TileClient* t;
        auto itr = tiles.find(tileId);
        if(itr != tiles.end())
        {
            t = itr->second;
        }
        // If not, create it, load it and update it
        else
        {
            t = new TileClient();
            t->clientLoad(this, &tileimage, &font);
            tiles.insert(std::pair<unsigned int, TileClient*>(tileId, t));
        }
        t->handleSnapshot(packet, full);
    }

    // Update all zombies
    sf::Uint32 zombieCount;
    packet >> zombieCount;
    for(unsigned int i=0; i < zombieCount; i++)
    {
        unsigned int zombieId;
        packet >> zombieId;

        // If zombie exists, update it
        ZombieClient* z;
        auto itr = zombies.find(zombieId);
        if(itr != zombies.end())
        {
            z = itr->second;
        }
        // If not, create it, load it and update it
        else
        {
            z = new ZombieClient();
            z->clientLoad(this, &zombieimage);
            zombies.insert(std::pair<unsigned int, ZombieClient*>(zombieId, z));
        }
        z->handleSnapshot(packet, full);

        // Nab player
        if(z->fixedState.clientId == state->clientId)
        {
            playerCharacter = z;
        }
    }

}