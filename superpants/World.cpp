#include "World.hpp"
#include "Zombie.hpp"
#include "Player.hpp"
#include "../bitengine/Network/RemoteClient.hpp"
#include "../bitengine/Network/ServerPacket.hpp"
#include "../bitengine/Network/ClientPacket.hpp"
#include "../bitengine/Math/Math.hpp"
#include "../ResourcePath.h"
#include "SFML/Network.hpp"

World::World()
{
}

World::~World()
{
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
    unsigned int zcount = 3;
    zombies.resize(zcount);
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

void World::prepareSnapshot(bit::ServerPacket &packet)
{
    sf::Uint32 zombieCount = zombies.size();
    packet << zombieCount;

    for(unsigned int i=0; i < zombies.size(); i++)
    {
        sf::Uint32 zombieId = i;
        packet << zombieId;
        zombies[i]->prepareSnapshot(packet);
    }
}