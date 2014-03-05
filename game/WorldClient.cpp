#include "WorldClient.hpp"
#include "TileClient.hpp"
#include "GameplayState.hpp"
#include "characters/ZombieClient.hpp"
#include "characters/OgreClient.hpp"
#include "structures/WallClient.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/Network.hpp"
#include "../ResourcePath.h"
#include "SFML/Network.hpp"
#include <map>

WorldClient::WorldClient()
    : state(NULL)
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
    for(unsigned int i=0; i < walls.size(); i++)
    {
        delete walls[i];
    }
}

void WorldClient::load(GameplayState* _state)
{
    state = _state;
    zombieimage.loadFromFile(resourcePath() + "Zombie.png");
    ogreimage.loadFromFile(resourcePath() + "Ogre.png");
    tileimage.loadFromFile(resourcePath() + "Water.png");
    wallimage.loadFromFile(resourcePath() + "Wall.png");
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
    for(auto iterator = ogres.begin(); iterator != ogres.end(); iterator++)
    {
        iterator->second->clientUpdate(gameTime);
    }
    for(auto iterator = walls.begin(); iterator != walls.end(); iterator++)
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
    for(auto iterator = ogres.begin(); iterator != ogres.end(); iterator++)
    {
        iterator->second->clientDraw(window, gameTime);
    }
    for(auto iterator = walls.begin(); iterator != walls.end(); iterator++)
    {
        iterator->second->clientDraw(window, gameTime);
    }
}

void WorldClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    // Tiles
    sf::Uint32 tileCount;
    packet >> tileCount;
    for(unsigned int i=0; i < tileCount; i++)
    {
        unsigned int tileId;
        packet >> tileId;

        // If exists, update it
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

    // Zombies
    sf::Uint32 zombieCount;
    packet >> zombieCount;
    for(unsigned int i=0; i < zombieCount; i++)
    {
        unsigned int zombieId;
        packet >> zombieId;

        // If exists, update it
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
    }

    // Ogres
    sf::Uint32 ogreCount;
    packet >> ogreCount;
    for(unsigned int i=0; i < ogreCount; i++)
    {
        unsigned int ogreId;
        packet >> ogreId;

        // If exists, update it
        OgreClient* o;
        auto itr = ogres.find(ogreId);
        if(itr != ogres.end())
        {
            o = itr->second;
        }
        // If not, create it, load it and update it
        else
        {
            o = new OgreClient();
            o->clientLoad(this, &ogreimage);
            ogres.insert(std::pair<unsigned int, OgreClient*>(ogreId, o));
        }
        o->handleSnapshot(packet, full);
    }

    // Walls
    sf::Uint32 wallCount;
    packet >> wallCount;
    for(unsigned int i=0; i < wallCount; i++)
    {
        unsigned int wallId;
        packet >> wallId;

        // If exists, update it
        WallClient* w;
        auto itr = walls.find(wallId);
        if(itr != walls.end())
        {
            w = itr->second;
        }
        // If not, create it, load it and update it
        else
        {
            w = new WallClient();
            w->clientLoad(this, &wallimage);
            walls.insert(std::pair<unsigned int, WallClient*>(wallId, w));
        }
        w->handleSnapshot(packet, full);
    }
}