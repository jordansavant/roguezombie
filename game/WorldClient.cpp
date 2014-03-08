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
    : state(NULL), tilePool(), zombiePool(), ogrePool(), wallPool(), hoveredTile(NULL)
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
    for(unsigned int i=0; i < ogres.size(); i++)
    {
        delete ogres[i];
    }
    for(unsigned int i=0; i < walls.size(); i++)
    {
        delete walls[i];
    }
}

void WorldClient::load(GameplayState* _state)
{
    state = _state;

    font.loadFromFile(resourcePath() + "Agency.ttf");
    state->game->spriteLoader->loadSprites(resourcePath() + "spritesheet_01.csv");
    texture_spritesheet_01.loadFromFile(resourcePath() + "spritesheet_01.png");
    vertexMap_01.load(&texture_spritesheet_01, sf::PrimitiveType::Quads);

    // Fill pools
    WorldClient* w = this;
    tilePool.factoryMethod = [w] () -> TileClient* {
        TileClient* t = new TileClient();
        t->clientLoad(w);
        return t;
    };
    tilePool.add(200);
    zombiePool.factoryMethod = [w] () -> ZombieClient* {
        ZombieClient* t = new ZombieClient();
        t->clientLoad(w);
        return t;
    };
    zombiePool.add(50);
    ogrePool.factoryMethod = [w] () -> OgreClient* {
        OgreClient* t = new OgreClient();
        t->clientLoad(w);
        return t;
    };
    ogrePool.add(10);
    wallPool.factoryMethod = [w] () -> WallClient* {
        WallClient* t = new WallClient();
        t->clientLoad(w);
        return t;
    };
    wallPool.add(100);
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

void WorldClient::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    bit::Game::depthTestBegin();

    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = vertexMap_01.texture;

    // draw the vertex arrays z-sorted
    target.draw(vertexMap_01.vertexArray, states);

    bit::Game::depthTestEnd();
}

void WorldClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    // Update / Create all entities
    unsigned int tileCount;
    packet >> tileCount;
    for(unsigned int i=0; i < tileCount; i++)
    {
        unpackNetworkEntity<TileClient>(packet, full, tiles, tilePool);

        // unpack body
        unsigned int bodyType;
        packet >> bodyType;
        switch(static_cast<Body::Type>(bodyType))
        {
            case Body::Type::Character:
            {
                unsigned int characterType;
                packet >> characterType;
                switch(static_cast<Character::Type>(characterType))
                {
                    case Character::Type::Zombie:
                        unpackNetworkEntity<ZombieClient>(packet, full, zombies, zombiePool);
                        break;
                    case Character::Type::Ogre:
                        unpackNetworkEntity<OgreClient>(packet, full, ogres, ogrePool);
                        break;
                }
                break;
            }
            case Body::Type::Structure:
            {
                unsigned int structureType;
                packet >> structureType;
                switch(static_cast<Structure::Type>(structureType))
                {
                    case Structure::Type::Wall:
                        unpackNetworkEntity<WallClient>(packet, full, walls, wallPool);
                        break;
                }
                break;
            }
            default: case Body::Type::None: break;
        }
    }

    // Delete missing entities
    diffNetworkEntity<TileClient>(tiles, tilePool);
    diffNetworkEntity<ZombieClient>(zombies, zombiePool);
    diffNetworkEntity<OgreClient>(ogres, ogrePool);
    diffNetworkEntity<WallClient>(walls, wallPool);
}