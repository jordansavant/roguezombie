#include "WorldClient.hpp"
#include "TileClient.hpp"
#include "GameplayState.hpp"
#include "characters/ZombieClient.hpp"
#include "characters/OgreClient.hpp"
#include "structures/WallClient.hpp"
#include "structures/DoorClient.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/Network.hpp"
#include "../ResourcePath.h"
#include "SFML/Network.hpp"
#include <map>

WorldClient::WorldClient()
    : state(NULL), tilePool(), zombiePool(), ogrePool(), wallPool(), hoveredTile(NULL), playerCharacter(NULL)
{
}

WorldClient::~WorldClient()
{
    for(unsigned int i=0; i < runners.size(); i++)
    {
        delete runners[i];
    }
}

void WorldClient::load(GameplayState* _state)
{
    state = _state;

    // Assets
    state->game->spriteLoader->loadSprites(resourcePath() + "spritesheet_01.csv");
    texture_spritesheet_01_unsmooth.loadFromFile(resourcePath() + "spritesheet_01.png");
    texture_spritesheet_01_smooth.loadFromFile(resourcePath() + "spritesheet_01.png");
    texture_spritesheet_01_smooth.setSmooth(true);
    vertexMap_01.load(&texture_spritesheet_01_unsmooth, sf::PrimitiveType::Quads);

    // Minimap
    minimap.load(this, texture_spritesheet_01_smooth);

    // Load game runners
    runners.push_back(new WorldClientRunner<TileClient>(this, &tiles, &tilePool, 2000));
    runners.push_back(new WorldClientRunner<ZombieClient>(this, &zombies, &zombiePool, 10));
    runners.push_back(new WorldClientRunner<OgreClient>(this, &ogres, &ogrePool, 10));
    runners.push_back(new WorldClientRunner<WallClient>(this, &walls, &wallPool, 500));
    runners.push_back(new WorldClientRunner<DoorClient>(this, &doors, &doorPool, 20));

    // Fill pools
    for(unsigned int i=0; i < runners.size(); i++)
    {
        runners[i]->buildPool();
    }
}

void WorldClient::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    // Update
    for(unsigned int i=0; i < runners.size(); i++)
    {
        runners[i]->update(window, gameTime);
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
        bool addMini = true;
        TileClient* t = unpackNetworkEntity<TileClient>(packet, full, tiles, tilePool);

        // unpack body
        unsigned int bodyType;
        packet >> bodyType;
        switch(static_cast<Body::Type>(bodyType))
        {
            case Body::Type::Character:
            {
                Character* c;

                unsigned int characterType;
                packet >> characterType;
                switch(static_cast<Character::Type>(characterType))
                {
                    case Character::Type::Zombie:
                        c = unpackNetworkEntity<ZombieClient>(packet, full, zombies, zombiePool);
                        break;
                    case Character::Type::Ogre:
                        c = unpackNetworkEntity<OgreClient>(packet, full, ogres, ogrePool);
                        break;
                }

                // Player character
                if(c->fixedState.clientId == state->clientId)
                {
                    playerCharacter = c;
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
                        addMini = false;
                        break;
                    case Structure::Type::Door:
                        unpackNetworkEntity<DoorClient>(packet, full, doors, doorPool);
                        break;
                }
                break;
            }
            default:
            case Body::Type::None:
                break;
        }

        if(addMini)
        {
            minimap.addPoint(t->fixedState.id, t->fixedState.x, t->fixedState.y);
        }
    }

    // Delete missing entities
    for(unsigned int i=0; i < runners.size(); i++)
    {
        runners[i]->diffNetwork();
    }
}