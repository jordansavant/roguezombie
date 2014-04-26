#include "Level.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "Light.hpp"
#include "LevelRunner.hpp"
#include "characters/Zombie.hpp"
#include "characters/Ogre.hpp"
#include "structures/Wall.hpp"
#include "structures/Door.hpp"
#include "structures/Chest.hpp"
#include "levels/Interior.hpp"
#include "GameplayServer.hpp"
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

Level::Level()
    : server(NULL), id(0), tileWidth(0), tileHeight(0), tileRows(0), tileColumns(0), tileCount(0)
{
}

Level::~Level()
{
    for(unsigned int i=0; i < runners.size(); i++)
    {
        delete runners[i];
    }
}

/*
 * Game Logic
 */

void Level::load(GameplayServer* _server, unsigned int _id, const int* t_array, int t_rows, int t_cols)
{
    server = _server;
    id = _id;

    // Runners
    runners.push_back(new LevelRunner<Tile>(this, &tiles));
    runners.push_back(new LevelRunner<Zombie>(this, &zombies));
    runners.push_back(new LevelRunner<Ogre>(this, &ogres));
    runners.push_back(new LevelRunner<Wall>(this, &walls));
    runners.push_back(new LevelRunner<Door>(this, &doors));
    runners.push_back(new LevelRunner<Chest>(this, &chests));
    runners.push_back(new LevelRunner<Light>(this, &lights));

    // Map
    tileRows = t_rows;
    tileColumns = t_cols;
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
            int logic = t_array[index];
            Tile* t = tiles[index];

            switch(logic)
            {
                case Interior::Spawn::Wall:
                {
                    Wall* w = new Wall();
                    w->load(this, server->getNextBodyId(), t->schema.x, t->schema.y);
                    walls.push_back(w);
                    break;
                }
                case Interior::Spawn::Zombie:
                {
                    Zombie* z = new Zombie();
                    z->load(this, server->getNextBodyId(), t->schema.x, t->schema.y);
                    zombies.push_back(z);
                    break;
                }
                case Interior::Spawn::Ogre:
                {
                    Ogre* o = new Ogre();
                    o->load(this, server->getNextBodyId(), t->schema.x, t->schema.y);
                    ogres.push_back(o);
                    break;
                }
                case Interior::Spawn::Light:
                {
                    sf::Color c = sf::Color::Red;
                    Light* l = new Light();
                    l->load(this, t->schema.x, t->schema.y, 4, c, .66);
                    lights.push_back(l);
                    break;
                }
                case Interior::Spawn::Door:
                {
                    Door* d = new Door();
                    d->load(this, server->getNextBodyId(), t->schema.x, t->schema.y);
                    doors.push_back(d);
                    break;
                }
                case Interior::Spawn::Chest:
                {
                    Chest* d = new Chest();
                    d->load(this, server->getNextBodyId(), t->schema.x, t->schema.y);
                    chests.push_back(d);
                    break;
                }
                case Interior::Spawn::PortalTo2:
                {
                    Level* l = this;
                    t->onBodyEnter.push_back([l] (Tile* t, Body* b){
                        if(b->schema.type == Body::Type::Character)
                        {
                            Character* c = static_cast<Character*>(b);
                            if(c->schema.isPlayerCharacter)
                            {
                                Player* p = l->players[c->schema.clientId];
                                l->server->movePlayerToLevel(p, l->id, 1);
                            }
                        }
                    });
                    break;
                }
                case Interior::Spawn::PortalTo1:
                {
                    Level* l = this;
                    t->onBodyEnter.push_back([l] (Tile* t, Body* b){
                        if(b->schema.type == Body::Type::Character)
                        {
                            Character* c = static_cast<Character*>(b);
                            if(c->schema.isPlayerCharacter)
                            {
                                Player* p = l->players[c->schema.clientId];
                                l->server->movePlayerToLevel(p, l->id, 0);
                            }
                        }
                    });
                    break;
                }
            }
        }
    }
}

void Level::update(sf::Time &gameTime)
{
    // Update entities
    for(unsigned int i=0; i < runners.size(); i++)
    {
        runners[i]->update(gameTime);
    }
}

/*
 * Player Management
 */

bool Level::createPlayer(Player* player)
{
    // Create character
    Zombie* zombie = new Zombie();
    zombie->load(this, server->getNextBodyId(), 0, 0);

    // Assign Character
    player->setCharacter(zombie);
    zombie->setControllingPlayer(player);

    // Create light source for character's vision
    Light* light = new Light();
    light->load(this, zombie->Body::schema.x, zombie->Body::schema.y, 12, sf::Color(220, 255, 175), .4);
    zombie->lights.push_back(light);
    Light* orbLight = new Light();
    orbLight->load(this, zombie->Body::schema.x, zombie->Body::schema.y, 4, sf::Color::White, .6);
    zombie->lights.push_back(orbLight);

    // Add player to this level's management
    if(!addPlayer(player))
    {
        deletePlayer(player);

        return false;
    }

    return true;
}

bool Level::addPlayer(Player* player)
{
    // Connect to Level
    player->setLevel(this);
    players[player->clientId] = player;

    // Push character into management list
    player->character->level = this;
    switch(player->character->schema.type)
    {
        case Character::Type::Zombie:
            zombies.push_back(static_cast<Zombie*>(player->character));
            break;
        case Character::Type::Ogre:
            ogres.push_back(static_cast<Ogre*>(player->character));
            break;
    }

    // Manage lights for the character
    for(unsigned int i=0; i < player->character->lights.size(); i++)
    {
        player->character->lights[i]->level = this;
        lights.push_back(player->character->lights[i]);
    }

    // Add body to tiles
    if(!player->character->moveToPosition(tiles[44]->schema.x, tiles[44]->schema.y))
    {
        removePlayer(player);

        return false;
    }

    return true;
}

void Level::removePlayer(Player* player)
{
    if(players.find(player->clientId) != players.end())
	{
        // Disconnect from Level
        player->setLevel(NULL);
        players.erase(player->clientId);

        // Remove body from tiles
        std::vector<Tile*> tiles;
        getTilesWithinRectangle(player->character->Body::schema.x, player->character->Body::schema.y, player->character->Body::schema.width, player->character->Body::schema.height, tiles);
        for(unsigned int i=0; i < tiles.size(); i++)
        {
            if(tiles[i]->body && tiles[i]->body == player->character)
            {
                tiles[i]->setOccupyingBody(NULL);
            }
        }

        // Remove lights from management list
        for(unsigned int i=0; i < player->character->lights.size(); i++)
        {
            lights.erase(std::remove(lights.begin(), lights.end(), player->character->lights[i]), lights.end());
        }

        // Remove character from management list
        switch(player->character->schema.type)
        {
            case Character::Type::Zombie:
                zombies.erase(std::remove(zombies.begin(), zombies.end(), static_cast<Zombie*>(player->character)), zombies.end());
                break;
            case Character::Type::Ogre:
                ogres.erase(std::remove(ogres.begin(), ogres.end(), static_cast<Ogre*>(player->character)), ogres.end());
                break;
        }
	}
}

void Level::deletePlayer(Player* player)
{
    // Remove player
    removePlayer(player);

    // Delete light source for character's vision
    for(unsigned int i=0; i < player->character->lights.size(); i++)
    {
        delete player->character->lights[i];
    }

    // Delete character
    delete player->character;

    // Delete player
    delete player;
}

/*
 * Tile Positioning and Pathfinding
 */

Tile* Level::getTileAtIndices(int x, int y)
{
    unsigned int index = x + (tileColumns * y);

    if(index < tiles.size())
        return tiles[index];
    return NULL;
}

bool Level::isCoordinateInMap(float x, float y)
{
    if(x >= mapWidth || x < 0)
        return false;
    if(y >= mapHeight || y < 0)
        return false;

    return true;
}

Tile* Level::getTileAtPosition(float x, float y)
{
    if(!isCoordinateInMap(x, y))
        return NULL;

    unsigned int tx = (unsigned int)std::floor((float)x / (float)tileWidth);
    unsigned int ty = (unsigned int)std::floor((float)y / (float)tileHeight);
    unsigned int index = tx + (tileColumns * ty);

    return tiles[index];
}

void Level::getTilesWithinRectangle(float left, float top, float width, float height, std::vector<Tile*> &fill)
{
    // Get the total size of the tiles
    int recTileWidth = (int)width / tileWidth;
    int recTileHeight = (int)height / tileHeight;
    int totalTiles = recTileWidth * recTileHeight;

    for(int x = left; x < left + width; x += tileWidth)
    {
        for(int y = top; y < top + height; y += tileHeight)
        {
            Tile* t = getTileAtPosition(x, y);
            if(t)
            {
                fill.push_back(t);
            }
        }
    }
}

void Level::getShortestPath(float startX, float startY, float endX, float endY, std::function<bool(Tile*)> isBlocked, std::function<void(Tile*, std::vector<Tile*>&)> getNeighbors, std::vector<Tile*> &fill)
{
    // Translate start and finish to tiles
    Tile* closestStartNode = getTileAtPosition(startX, startY);
    Tile* closestFinishNode = getTileAtPosition(endX, endY);

    // Return empty list if there is no path
    if (closestStartNode == NULL || closestFinishNode == NULL || isBlocked(closestStartNode) || isBlocked(closestFinishNode))
        return;

    bit::Astar::pathfind(closestStartNode, closestFinishNode, isBlocked, getNeighbors, fill);
}

void Level::getCardinalTiles(Tile* tile, std::vector<Tile*> &fill)
{
    getCardinalTilesNullsafe(tile, fill, true);
}

void Level::getCardinalTilesNullsafe(Tile* tile, std::vector<Tile*> &fill, bool nullsafe)
{
    Tile* top = getTileAtPosition(tile->schema.x, tile->schema.y - tile->schema.height);
    Tile* bottom = getTileAtPosition(tile->schema.x, tile->schema.y + tile->schema.height);
    Tile* left = getTileAtPosition(tile->schema.x - tile->schema.width, tile->schema.y);
    Tile* right = getTileAtPosition(tile->schema.x + tile->schema.width, tile->schema.y);

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

void Level::raycastTiles(float startX, float startY, float endX, float endY, std::function<bool(Tile*)> inspect)
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

void Level::handlePlayerCommand(bit::ClientPacket &packet, bit::RemoteClient &client, Command::Type commandType)
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
                player->character->pathToPosition(t->schema.x, t->schema.y);
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
                raycastTiles(t->schema.x, t->schema.y,  p->character->Body::schema.x, p->character->Body::schema.y, [] (Tile* t) -> bool {
                    t->schema.illumination = 1.0f;
                    return false;
                });
            }
            break;
        }
	}
}

void Level::prepareSnapshot(bit::ServerPacket &packet, bit::RemoteClient& client, bool full)
{
    Player* p = players[client.id];

    // Get a subset of visible tiles for the player within a radius of tiles
    std::vector<Tile*> visibles;
    Level* w = this;
    bit::Shadowcaster::computeFoV(p->character->Body::schema.x / tileWidth, p->character->Body::schema.y / tileHeight, tileColumns, tileRows, 30,
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
            return (t && t->body && t->body->blockFoV);
        }
    );

    // indicate number of tiles
    packet << sf::Uint32(visibles.size());

    for(unsigned int i=0; i < visibles.size(); i++)
    {
        Tile* t = visibles[i];

        // shove a tile type and tile info on there
        sf::Uint32 tileId = t->schema.id;
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
        switch(b->schema.type)
        {
            case Body::Type::Character:
            {
                Character* c = static_cast<Character*>(b);
                switch(c->schema.type)
                {
                    case Character::Type::Zombie:
                        packNetworkBody<Zombie, Character>(packet, full, c, b->schema.type, c->schema.type);
                        break;
                    case Character::Type::Ogre:
                        packNetworkBody<Ogre, Character>(packet, full, c, b->schema.type, c->schema.type);
                        break;
                }
                break;
            }

            case Body::Type::Structure:
            {
                Structure* s = static_cast<Structure*>(b);
                switch(s->schema.type)
                {
                    case Structure::Type::Wall:
                        packNetworkBody<Wall, Structure>(packet, full, s, b->schema.type, s->schema.type);
                        break;
                    case Structure::Type::Door:
                        packNetworkBody<Door, Structure>(packet, full, s, b->schema.type, s->schema.type);
                        break;
                    case Structure::Type::Chest:
                        packNetworkBody<Chest, Structure>(packet, full, s, b->schema.type, s->schema.type);
                        break;
                }
                break;
            }
            case Body::Type::None:
                break;
        }
    }
}