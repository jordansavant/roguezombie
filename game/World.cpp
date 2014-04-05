#include "World.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "Light.hpp"
#include "WorldRunner.hpp"
#include "characters/Zombie.hpp"
#include "characters/Ogre.hpp"
#include "structures/Wall.hpp"
#include "structures/Door.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/System.hpp"
#include "../bitengine/Intelligence.hpp"
#include "../bitengine/Intelligence/Shadowcaster.hpp"
#include "../ResourcePath.h"
#include <functional>
#include <sstream>
#include <map>

World::World()
    : id(0), tileWidth(0), tileHeight(0), tileRows(0), tileColumns(0), tileCount(0)
{
}

World::~World()
{
    for(unsigned int i=0; i < runners.size(); i++)
    {
        delete runners[i];
    }
}

/*
 * Game Logic
 */

void World::load(unsigned int _id)
{
    id = _id;

    // Runners
    runners.push_back(new WorldRunner<Tile>(this, &tiles));
    runners.push_back(new WorldRunner<Zombie>(this, &zombies));
    runners.push_back(new WorldRunner<Ogre>(this, &ogres));
    runners.push_back(new WorldRunner<Wall>(this, &walls));
    runners.push_back(new WorldRunner<Door>(this, &doors));
    runners.push_back(new WorldRunner<Light>(this, &lights));
    
    // Tiles
    const int tileArray[] =
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 1, 0, 0, 5, 0, 0, 0, 0, 0, 0, 0,   0, 0, 5, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1,
        1, 0, 3, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,   0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1,
        1, 0, 4, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,   0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1,
        1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,   0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        1, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,   0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        1, 0, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1,   1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        1, 0, 4, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        1, 1, 5, 1, 1, 0, 0, 0, 0, 0, 5, 0, 0, 0, 5,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
                                                      
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 5, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 5, 1, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 3, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 1, 1,
        1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,   2, 0, 0, 0, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    };

    tileRows = 15 * 2;
    tileColumns = 15 * 2;
    tileCount = tileRows * tileColumns;
    tileWidth = 32;
    tileHeight = 32;
    mapWidth = tileWidth * tileColumns;
    mapHeight = tileHeight * tileRows;
    tiles.resize(tileCount, NULL);

    // Load Tiles
    for (unsigned int j = 0; j < tileRows; ++j)
    {
        for (unsigned int i = 0; i < tileColumns; ++i)
        {
            // get the current tile information
            int index = i + j * tileColumns;
            Tile::Type tileType = Tile::Type::Ground;

            // build the quad and its position on the map
            float originZ = 0;
            float originX = i * tileWidth;
            float originY = j * tileHeight;

            // Load our tile
            Tile* t = new Tile();
            t->load(this, index, tileType, originX, originY, tileWidth, tileHeight);
            tiles[index] = t;
        }
    }

    // Load entities
    for (unsigned int j = 0; j < tileRows; ++j)
    {
        for (unsigned int i = 0; i < tileColumns; ++i)
        {
            int index = i + j * tileColumns;
            int logic = tileArray[index];
            Tile* t = tiles[index];

            switch(logic)
            {
                case 1:
                {
                    Wall* w = new Wall();
                    w->load(this, walls.size(), t->fixedState.x, t->fixedState.y);
                    walls.push_back(w);
                    break;
                }
                case 2:
                {
                    Zombie* z = new Zombie();
                    z->load(this, zombies.size(), t->fixedState.x, t->fixedState.y);
                    zombies.push_back(z);
                    break;
                }
                case 3:
                {
                    Ogre* o = new Ogre();
                    o->load(this, ogres.size(), t->fixedState.x, t->fixedState.y);
                    ogres.push_back(o);
                    break;
                }
                case 4:
                {
                    sf::Color c = sf::Color::Red;
                    Light* l = new Light();
                    l->load(this, t->fixedState.x, t->fixedState.y, 4, c, .66);
                    lights.push_back(l);
                    break;
                }
                case 5:
                {
                    Door* d = new Door();
                    d->load(this, doors.size(), t->fixedState.x, t->fixedState.y);
                    doors.push_back(d);
                    break;
                }
            }
        }
    }
}

void World::update(sf::Time &gameTime)
{
    // Update entities
    for(unsigned int i=0; i < runners.size(); i++)
    {
        runners[i]->update(gameTime);
    }
}

void World::createPlayer(Player* player)
{
    if(players.find(player->clientId) == players.end())
	{
        // Connect to World
        player->setWorld(this);
        players[player->clientId] = player;

        // Spawn character
		Zombie* zombie = new Zombie();
        zombie->load(this, zombies.size(), tiles[44]->fixedState.x, tiles[44]->fixedState.y);
		zombies.push_back(zombie);

        // Assign Characer
        player->setCharacter(zombie);
        zombie->setControllingPlayer(player);

        // Spawn light source for character's vision
        Light* light = new Light();
        light->load(this, zombie->Body::deltaState.x, zombie->Body::deltaState.y, 12, sf::Color(220, 255, 175), .4);
        lights.push_back(light);
        zombie->lights.push_back(light);

        // Spawn closer orb light
        Light* orbLight = new Light();
        orbLight->load(this, zombie->Body::deltaState.x, zombie->Body::deltaState.y, 4, sf::Color::White, .6);
        lights.push_back(orbLight);
        zombie->lights.push_back(orbLight);
	}
}

/*
 * Tile Positioning and Pathfinding
 */

Tile* World::getTileAtIndices(int x, int y)
{
    unsigned int index = x + (tileColumns * y);

    if(index < tiles.size())
        return tiles[index];
    return NULL;
}

bool World::isCoordinateInMap(float x, float y)
{
    if(x >= mapWidth || x < 0)
        return false;
    if(y >= mapHeight || y < 0)
        return false;

    return true;
}

Tile* World::getTileAtPosition(float x, float y)
{
    if(!isCoordinateInMap(x, y))
        return NULL;

    unsigned int tx = (unsigned int)std::floor((float)x / (float)tileWidth);
    unsigned int ty = (unsigned int)std::floor((float)y / (float)tileHeight);
    unsigned int index = tx + (tileColumns * ty);

    return tiles[index];
}

void World::getTilesWithinRectangle(float left, float top, float width, float height, std::vector<Tile*> &fill)
{
    // Get the total size of the tiles
    int recTileWidth = (int)width / tileWidth;
    int recTileHeight = (int)height / tileHeight;
    int totalTiles = recTileWidth * recTileHeight;

    fill.resize(fill.size() + totalTiles, NULL);

    int i=0;
    for(int x = left; x < left + width; x += tileWidth)
    {
        for(int y = top; y < top + height; y += tileHeight)
        {
            fill[i] = getTileAtPosition(x, y);
            i++;
        }
    }
}

void World::getShortestPath(float startX, float startY, float endX, float endY, std::function<bool(Tile*)> isBlocked, std::function<void(Tile*, std::vector<Tile*>&)> getNeighbors, std::vector<Tile*> &fill)
{
    // Translate start and finish to tiles
    Tile* closestStartNode = getTileAtPosition(startX, startY);
    Tile* closestFinishNode = getTileAtPosition(endX, endY);

    // Return empty list if there is no path
    if (closestStartNode == NULL || closestFinishNode == NULL || isBlocked(closestStartNode) || isBlocked(closestFinishNode))
        return;

    bit::Astar::pathfind(closestStartNode, closestFinishNode, isBlocked, getNeighbors, fill);
}

void World::getCardinalTiles(Tile* tile, std::vector<Tile*> &fill)
{
    getCardinalTiles(tile, fill, true);
}

void World::getCardinalTiles(Tile* tile, std::vector<Tile*> &fill, bool nullsafe)
{
    Tile* top = getTileAtPosition(tile->fixedState.x, tile->fixedState.y - tile->fixedState.height);
    Tile* bottom = getTileAtPosition(tile->fixedState.x, tile->fixedState.y + tile->fixedState.height);
    Tile* left = getTileAtPosition(tile->fixedState.x - tile->fixedState.width, tile->fixedState.y);
    Tile* right = getTileAtPosition(tile->fixedState.x + tile->fixedState.width, tile->fixedState.y);

    if(top)
        fill.push_back(top);
    else if(!nullsafe)
        fill.push_back(top);

    if(bottom)
        fill.push_back(bottom);
    else if(!nullsafe)
        fill.push_back(bottom);

    if(left)
        fill.push_back(left);
    else if(!nullsafe)
        fill.push_back(left);

    if(right)
        fill.push_back(right);
    else if(!nullsafe)
        fill.push_back(right);
}

void World::raycastTiles(float startX, float startY, float endX, float endY, std::function<bool(Tile*)> inspect)
{
    // Convert to tile coordinates
    unsigned int stx = (unsigned int)std::floor((float)startX / (float)tileWidth);
    unsigned int sty = (unsigned int)std::floor((float)startY / (float)tileHeight);
    unsigned int etx = (unsigned int)std::floor((float)endX / (float)tileWidth);
    unsigned int ety = (unsigned int)std::floor((float)endY / (float)tileHeight);

    std::vector<sf::Vector2i> line = bit::VectorMath::bresenhamLine(stx, sty, etx, ety);

    for(unsigned int i=0; i < line.size(); i++)
    {
        unsigned int index = line[i].x + line[i].y * tileColumns;
        if(index < tiles.size())
        {
            Tile* t = tiles[index];

            if(inspect(t))
            {
                return;
            }
        }
    }
}


/*
 * Networking
 */

void World::handlePlayerCommand(bit::ClientPacket &packet, bit::RemoteClient &client, Command::Type commandType)
{
	Player* player = players[client.id];
	sf::Vector2f d(0, 0);

	switch(commandType)
	{
        default:
            break;
		case Command::Type::PlayerMoveUp:
            player->character->moveUp();
			break;
		case Command::Type::PlayerMoveDown:
            player->character->moveDown();
			break;
		case Command::Type::PlayerMoveLeft:
            player->character->moveLeft();
			break;
		case Command::Type::PlayerMoveRight:
            player->character->moveRight();
			break;
        case Command::Type::PlayerClickTile:
        {
            unsigned int tileId;
            packet >> tileId;
            Tile* t = tiles[tileId];
            if(t)
            {
                player->character->pathToPosition(t->fixedState.x, t->fixedState.y);
            }
            break;
        }
        case Command::Type::PlayerRightClickTile:
        {
            unsigned int tileId;
            packet >> tileId;
            Tile* t = tiles[tileId];
            Player* p = players[client.id];
            if(t)
            {
                raycastTiles(t->fixedState.x, t->fixedState.y,  p->character->Body::deltaState.x, p->character->Body::deltaState.y, [] (Tile* t) -> bool {
                    t->deltaState.illumination = 1.0f;
                    return false;
                });
            }
            break;
        }
	}
}

void World::prepareSnapshot(bit::ServerPacket &packet, bit::RemoteClient& client, bool full)
{
    Player* p = players[client.id];

    // Get a subset of visible tiles for the player within a radius of tiles
    std::vector<Tile*> visibles;
    World* w = this;
    bit::Shadowcaster::computeFoV(p->character->Body::deltaState.x / tileWidth, p->character->Body::deltaState.y / tileHeight, tileRows, tileColumns, 30,
        [&visibles, w] (int x, int y, float radius)
        {
            Tile* t = w->getTileAtIndices(x, y);
            if(t->metadata_shadowcastId != bit::Shadowcaster::shadowcastId)
            {
                t->metadata_shadowcastId = bit::Shadowcaster::shadowcastId;
                visibles.push_back(t);
            }
        },
        [&visibles, w] (int x, int y) -> bool
        {
            Tile* t = w->getTileAtPosition(x * w->tileWidth, y * w->tileHeight);
            return (t && t->body && t->body->fixedState.type == Body::Type::Structure);
        }
    );

    // indicate number of tiles
    packet << sf::Uint32(visibles.size());

    for(unsigned int i=0; i < visibles.size(); i++)
    {
        Tile* t = visibles[i];

        // shove a tile type and tile info on there
        sf::Uint32 tileId = t->fixedState.id;
        packet << tileId;
        t->prepareSnapshot(packet, full);

        Body* b;
        // If no body or door, say no body
        if(!t->body && !t->door)
        {
            packet << sf::Uint32(Body::Type::None);
            continue;
        }
        // If there is a body, use it
        else if(t->body)
        {
            b = t->body;
        }
        // If there is a door, use it
        else
        {
            b = t->door;
        }

        // break open body
        switch(b->fixedState.type)
        {
            case Body::Type::Character:
            {
                Character* c = static_cast<Character*>(b);
                switch(c->fixedState.type)
                {
                    case Character::Type::Zombie:
                        packNetworkBody<Zombie, Character>(packet, full, c, b->fixedState.type, c->fixedState.type);
                        break;
                    case Character::Type::Ogre:
                        packNetworkBody<Ogre, Character>(packet, full, c, b->fixedState.type, c->fixedState.type);
                        break;
                }
                break;
            }

            case Body::Type::Structure:
            {
                Structure* s = static_cast<Structure*>(b);
                switch(s->fixedState.type)
                {
                    case Structure::Type::Wall:
                        packNetworkBody<Wall, Structure>(packet, full, s, b->fixedState.type, s->fixedState.type);
                        break;
                    case Structure::Type::Door:
                        packNetworkBody<Door, Structure>(packet, full, s, b->fixedState.type, s->fixedState.type);
                        break;
                }
                break;
            }
        }
    }
}