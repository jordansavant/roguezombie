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
    sf::Uint32 tileCount;
    packet >> tileCount;
    for(unsigned int i=0; i < tileCount; i++)
    {
        unpackTile(packet, full);
        unpackBody(packet, full);
    }

    // Delete missing entities
    auto tile_itr = tiles.begin();
    while(tile_itr != tiles.end())
    {
        if (tile_itr->second->lastSnapshotId != state->lastSnapshotId)
        {
            tilePool.recycle(tile_itr->second);
            tiles.erase(tile_itr++);
        }
        else
        {
            ++tile_itr;
        }
    }
    auto zombie_itr = zombies.begin();
    while(zombie_itr != zombies.end())
    {
        if (zombie_itr->second->lastSnapshotId != state->lastSnapshotId)
        {
            zombiePool.recycle(zombie_itr->second);
            zombies.erase(zombie_itr++);
        }
        else
        {
            ++zombie_itr;
        }
    }
    auto ogre_itr = ogres.begin();
    while(ogre_itr != ogres.end())
    {
        if (ogre_itr->second->lastSnapshotId != state->lastSnapshotId)
        {
            ogrePool.recycle(ogre_itr->second);
            ogres.erase(ogre_itr++);
        }
        else
        {
            ++ogre_itr;
        }
    }
    auto wall_itr = walls.begin();
    while(wall_itr != walls.end())
    {
        if (wall_itr->second->lastSnapshotId != state->lastSnapshotId)
        {
            wallPool.recycle(wall_itr->second);
            walls.erase(wall_itr++);
        }
        else
        {
            ++wall_itr;
        }
    }
}

void WorldClient::unpackTile(bit::ServerPacket &packet, bool full)
{
    // Pull the tile id off
    unsigned int tileId;
    packet >> tileId;

    // Unpack the tile
    TileClient* t;
    auto itr = tiles.find(tileId);
    if(itr != tiles.end())
    {
        t = itr->second;
    }
    else
    {
        t = new TileClient();
        t->clientLoad(this);
        tiles.insert(std::pair<unsigned int, TileClient*>(tileId, t));
    }
    t->lastSnapshotId = state->lastSnapshotId; // update snapshot id
    t->handleSnapshot(packet, full);
}

void WorldClient::unpackBody(bit::ServerPacket &packet, bool full)
{
    // Get type
    unsigned int bodyType;
    packet >> bodyType;
    switch(bodyType)
    {
        default:
        case 777: // noBody
            break;
        case 333: // zombie
        {
            unsigned int zombieId;
            packet >> zombieId;
            ZombieClient* z;
            auto itr = zombies.find(zombieId);
            if(itr != zombies.end())
            {
                z = itr->second;
            }
            else
            {
                z = new ZombieClient();
                z->clientLoad(this);
                zombies.insert(std::pair<unsigned int, ZombieClient*>(zombieId, z));
            }
            z->lastSnapshotId = state->lastSnapshotId; // update snapshot id
            z->handleSnapshot(packet, full);

            break;
        }
        case 444: // ogre
        {
            unsigned int ogreId;
            packet >> ogreId;
            OgreClient* o;
            auto itr = ogres.find(ogreId);
            if(itr != ogres.end())
            {
                o = itr->second;
            }
            else
            {
                o = new OgreClient();
                o->clientLoad(this);
                ogres.insert(std::pair<unsigned int, OgreClient*>(ogreId, o));
            }
            o->lastSnapshotId = state->lastSnapshotId; // update snapshot id
            o->handleSnapshot(packet, full);

            break;
        }
        case 222: // wall
        {
            unsigned int wallId;
            packet >> wallId;
            WallClient* w;
            auto itr = walls.find(wallId);
            if(itr != walls.end())
            {
                w = itr->second;
            }
            else
            {
                w = new WallClient();
                w->clientLoad(this);
                walls.insert(std::pair<unsigned int, WallClient*>(wallId, w));
            }
            w->lastSnapshotId = state->lastSnapshotId; // update snapshot id
            w->handleSnapshot(packet, full);

            break;
        }
    }
}