#include "World.hpp"
#include "Zombie.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "../ResourcePath.h"
#include <map>

World::World()
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
}

void World::load()
{
    // Tiles
    const int tileArray[] =
    {
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
    };

    unsigned int tileCount = 4 * 4;
    unsigned int rows = 4, columns = 4;
    unsigned tileWidth = 64, tileHeight = 64;
    tiles.resize(tileCount, NULL);
    for (unsigned int j = 0; j < rows; ++j)
    {
        for (unsigned int i = 0; i < columns; ++i)
        {
            // get the current tile information
            int index = i + j * columns;
            Tile::Type tileType = static_cast<Tile::Type>(tileArray[index]);

            // build the quad and its position on the map
            float z = 0;
            float originX = i * tileWidth;
            float originY = j * tileHeight;

            // Load our tile
            Tile* t = new Tile();
            t->load(this, tileType, originX, originY, tileWidth, tileHeight);
            tiles[index] = t;
        }
    }

    // Zombies
    unsigned int zcount = 3;
    zombies.resize(zcount, NULL);
    for(unsigned int i=0; i < zcount; i++)
    {
        Zombie* z = new Zombie();
        z->load(this, 500 * bit::Math::randomFloat(), 500 * bit::Math::randomFloat());
        zombies[i] = z;
    }
}

void World::update(sf::Time &gameTime)
{
    for(unsigned int i=0; i < zombies.size(); i++)
    {
        zombies[i]->update(gameTime);
    }
}

void World::createPlayer(bit::RemoteClient &client)
{
	if(players.find(client.id) == players.end())
	{
		Zombie* zombie = new Zombie();
        zombie->load(this, 100, 100);
		zombie->isPlayerControlled = true;
		zombies.push_back(zombie);

		Player* player = new Player();
		player->load(this, zombie);

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
			d.y = -1;
			break;
		case Command::Type::PlayerMoveDown:
			d.y = 1;
			break;
		case Command::Type::PlayerMoveLeft:
			d.x = -1;
			break;
		case Command::Type::PlayerMoveRight:
			d.x = 1;
			break;
        case Command::Type::PlayerTeleport:
            packet >> player->zombie->deltaState.x >> player->zombie->deltaState.y;
            break;
	}
    player->zombie->updatePosition(d);
}

void World::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    // Tiles
    sf::Uint32 tileCount = tiles.size();
    packet << tileCount;

    for(unsigned int i=0; i < tiles.size(); i++)
    {
        sf::Uint32 tileId = i;
        packet << tileId;
        tiles[i]->prepareSnapshot(packet, full);
    }

    // Zombies
    sf::Uint32 zombieCount = zombies.size();
    packet << zombieCount;

    for(unsigned int i=0; i < zombies.size(); i++)
    {
        sf::Uint32 zombieId = i;
        packet << zombieId;
        zombies[i]->prepareSnapshot(packet, full);
    }
}