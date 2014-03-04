#include "World.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "characters/Zombie.hpp"
#include "structures/Wall.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/System.hpp"
#include "../ResourcePath.h"
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

    for(auto iterator = players.begin(); iterator != players.end(); iterator++)
    {
		delete iterator->second;
    }

    for(unsigned int i=0; i < walls.size(); i++)
    {
        delete walls[i];
    }
}

void World::load()
{
    // Tiles
    const int tileArray[] =
    {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    };

    tileRows = 10;
    tileColumns = 10;
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
            Tile::Type tileType = static_cast<Tile::Type>(tileArray[index]);

            // build the quad and its position on the map
            float originZ = 0;
            float originX = i * tileWidth;
            float originY = j * tileHeight;

            // Load our tile
            Tile* t = new Tile();
            t->load(this, index, tileType, originX, originY, tileWidth, tileHeight);
            tiles[index] = t;

            // Add a zombie randomly
            if(bit::Math::random(25) == 1)
            {
                Zombie* z = new Zombie();
                z->load(this, zombies.size(), t->fixedState.centerX, t->fixedState.centerY);
                zombies.push_back(z);
                t->setOccupyingBody(z);
            }
            // Add a wall randomly
            else if(bit::Math::random(25) == 1)
            {
                Wall* w = new Wall();
                w->load(this, walls.size(), t->fixedState.centerX, t->fixedState.centerY);
                walls.push_back(w);
                t->setOccupyingBody(w);
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
    for(unsigned int i=0; i < walls.size(); i++)
    {
        walls[i]->update(gameTime);
    }
}

void World::createPlayer(bit::RemoteClient &client)
{
	if(players.find(client.id) == players.end())
	{
		Zombie* zombie = new Zombie();
        zombie->load(this, zombies.size(), tiles[0]->fixedState.centerX, tiles[0]->fixedState.centerY);
		zombies.push_back(zombie);

		Player* player = new Player();
        player->load(this, zombie, client.id);
        zombie->setControllingPlayer(player);

		players.insert(std::pair<unsigned int, Player*>(client.id, player));
	}
}

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
        case Command::Type::PlayerTeleport:
            float x, y;
            packet >> x >> y;
            Tile* t = getTileAtPosition(x, y);
            if(t)
            {
                player->character->moveToTile(t);
            }
            break;
	}
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

    if(index < tileCount)
    {
        return tiles[index];
    }

    return NULL;
}

void World::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    // Tiles
    sf::Uint32 tileCount = tiles.size();
    packet << tileCount;

    for(unsigned int i=0; i < tiles.size(); i++)
    {
        sf::Uint32 tileId = tiles[i]->fixedState.id;
        packet << tileId;
        tiles[i]->prepareSnapshot(packet, full);
    }

    // Zombies
    sf::Uint32 zombieCount = zombies.size();
    packet << zombieCount;

    for(unsigned int i=0; i < zombies.size(); i++)
    {
        sf::Uint32 zombieId = zombies[i]->Body::fixedState.id;
        packet << zombieId;
        zombies[i]->prepareSnapshot(packet, full);
    }

    // Walls
    sf::Uint32 wallCount = walls.size();
    packet << wallCount;

    for(unsigned int i=0; i < walls.size(); i++)
    {
        sf::Uint32 wallId = walls[i]->Body::fixedState.id;
        packet << wallId;
        walls[i]->prepareSnapshot(packet, full);
    }
}