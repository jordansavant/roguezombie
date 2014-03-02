#include "World.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "characters/Zombie.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/System.hpp"
#include "../ResourcePath.h"
#include <sstream>
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
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0,
    };

    unsigned int rows = 5, columns = 5;
    unsigned int tileCount = rows * columns;
    unsigned tileWidth = 32, tileHeight = 32;
    tiles.resize(tileCount, NULL);
    for (unsigned int j = 0; j < rows; ++j)
    {
        for (unsigned int i = 0; i < columns; ++i)
        {
            // get the current tile information
            int index = i + j * columns;
            Tile::Type tileType = static_cast<Tile::Type>(tileArray[index]);

            // build the quad and its position on the map
            float originZ = 0;
            float originX = i * tileWidth;
            float originY = j * tileHeight;

            // Load our tile
            Tile* t = new Tile();
            t->load(this, tileType, originX, originY, tileWidth, tileHeight);
            tiles[index] = t;

            // Add a zombie randomly
            if(bit::Math::random(4) == 1)
            {
                Zombie* z = new Zombie();
                z->load(this, t);
                zombies.push_back(z);
            }
        }
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
        zombie->load(this, tiles[0]);
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
            player->zombie->updatePosition(d);
			break;
		case Command::Type::PlayerMoveDown:
			d.y = 1;
            player->zombie->updatePosition(d);
			break;
		case Command::Type::PlayerMoveLeft:
			d.x = -1;
            player->zombie->updatePosition(d);
			break;
		case Command::Type::PlayerMoveRight:
			d.x = 1;
            player->zombie->updatePosition(d);
			break;
        case Command::Type::PlayerTeleport:
            float x, y;
            packet >> x >> y;
            player->zombie->deltaState.x = x;
            player->zombie->deltaState.y = y;

            std::stringstream ss;
            ss << "Receieved " << x << " " << y;
            bit::Output::Debug(ss.str());
            break;
	}
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