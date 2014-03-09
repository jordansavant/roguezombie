#include "World.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "Light.hpp"
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
    : tileWidth(0), tileHeight(0), tileRows(0), tileColumns(0), tileCount(0)
{
}

World::~World()
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
    for(auto iterator = players.begin(); iterator != players.end(); iterator++)
    {
		delete iterator->second;
    }
    for(unsigned int i=0; i < walls.size(); i++)
    {
        delete walls[i];
    }
    for(unsigned int i=0; i < doors.size(); i++)
    {
        delete doors[i];
    }
    for(unsigned int i=0; i < lights.size(); i++)
    {
        delete lights[i];
    }
}

/*
 * Game Logic
 */

void World::load()
{
    // Tiles
    const int tileArray[] =
    {
        4, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 3, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
        0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0,
        0, 1, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 0,
        0, 0, 0, 0, 1, 2, 0, 1, 0, 0, 0, 0, 0, 1, 0,
        2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    tileRows = 15;
    tileColumns = 15;
    tileCount = tileRows * tileColumns;
    tileWidth = 32;
    tileHeight = 32;
    mapWidth = tileWidth * tileColumns;
    mapHeight = tileHeight * tileRows;
    tiles.resize(tileCount, NULL);
    for (unsigned int j = 0; j < tileRows; ++j)
    {
        for (unsigned int i = 0; i < tileColumns; ++i)
        {
            // get the current tile information
            int index = i + j * tileColumns;
            int logic = tileArray[index];
            Tile::Type tileType = Tile::Type::Ground;

            // build the quad and its position on the map
            float originZ = 0;
            float originX = i * tileWidth;
            float originY = j * tileHeight;

            // Load our tile
            Tile* t = new Tile();
            t->load(this, index, tileType, originX, originY, tileWidth, tileHeight);
            tiles[index] = t;

            switch(logic)
            {
                case 1:
                {
                    Wall* w = new Wall();
                    w->load(this, walls.size(), t->fixedState.x, t->fixedState.y);
                    walls.push_back(w);
                    t->setOccupyingBody(w);
                    break;
                }
                case 2:
                {
                    Zombie* z = new Zombie();
                    z->load(this, zombies.size(), t->fixedState.x, t->fixedState.y);
                    zombies.push_back(z);
                    t->setOccupyingBody(z);
                    break;
                }
                case 3:
                {
                    Ogre* o = new Ogre();
                    o->load(this, ogres.size(), t->fixedState.x, t->fixedState.y);
                    ogres.push_back(o);
                    t->setOccupyingBody(o);
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
                    t->setOccupyingBody(d);
                    break;
                }
            }
        }
    }
}

void World::update(sf::Time &gameTime)
{
    for(unsigned int i=0; i < tiles.size(); i++)
    {
        tiles[i]->update(gameTime);
    }
    for(unsigned int i=0; i < zombies.size(); i++)
    {
        zombies[i]->update(gameTime);
    }
    for(unsigned int i=0; i < ogres.size(); i++)
    {
        ogres[i]->update(gameTime);
    }
    for(unsigned int i=0; i < walls.size(); i++)
    {
        walls[i]->update(gameTime);
    }
    for(unsigned int i=0; i < doors.size(); i++)
    {
        doors[i]->update(gameTime);
    }
    for(unsigned int i=0; i < lights.size(); i++)
    {
        lights[i]->update(gameTime);
    }
}

void World::createPlayer(bit::RemoteClient &client)
{
	if(players.find(client.id) == players.end())
	{
        // Spawn character
		Zombie* zombie = new Zombie();
        zombie->load(this, zombies.size(), tiles[14]->fixedState.x, tiles[14]->fixedState.y);
		zombies.push_back(zombie);

        // Create Player
		Player* player = new Player();
        player->load(this, zombie, client.id);
		players.insert(std::pair<unsigned int, Player*>(client.id, player));
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
 * Field of View
 */

void World::shadowcastSetVisible(int x, int y, float distance)
{
    Tile* t = getTileAtPosition(x * tileWidth, y * tileHeight);
    if(t)
    {
        float currentLight = t->deltaState.illumination;
        float thisLight = (.1f + .9f * (1 - distance));
        float combinedLight = currentLight + thisLight;
        float newLight = bit::Math::clamp(combinedLight, currentLight, 1.0f);
        t->deltaState.illumination = newLight;
 //       t->deltaState.illumination = thisLight;
    }
}

bool World::shadowcastIsBlocked(int x, int y)
{
    Tile* t = getTileAtPosition(x * tileWidth, y * tileHeight);
    return (t && t->body && t->body->fixedState.type == Body::Type::Structure);
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

    // Get a subset of visible tiles for the player within 8 tiles
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
            return w->shadowcastIsBlocked(x, y);
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

        if(!t->body)
        {
            // If no body, say no body
            packet << sf::Uint32(Body::Type::None);
            continue;
        }

        // break open body
        switch(t->body->fixedState.type)
        {
            case Body::Type::Character:
            {
                Character* c = static_cast<Character*>(t->body);
                switch(c->fixedState.type)
                {
                    case Character::Type::Zombie:
                        packNetworkBody<Zombie, Character>(packet, full, c, t->body->fixedState.type, c->fixedState.type);
                        break;
                    case Character::Type::Ogre:
                        packNetworkBody<Ogre, Character>(packet, full, c, t->body->fixedState.type, c->fixedState.type);
                        break;
                }
                break;
            }

            case Body::Type::Structure:
            {
                Structure* s = static_cast<Structure*>(t->body);
                switch(s->fixedState.type)
                {
                    case Structure::Type::Wall:
                        packNetworkBody<Wall, Structure>(packet, full, s, t->body->fixedState.type, s->fixedState.type);
                        break;
                    case Structure::Type::Door:
                        packNetworkBody<Door, Structure>(packet, full, s, t->body->fixedState.type, s->fixedState.type);
                        break;
                }
                break;
            }
        }
    }
}