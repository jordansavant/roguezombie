#include "World.hpp"
#include "Zombie.hpp"
#include "Player.hpp"
#include "../bitengine/Game/RemoteClient.hpp"
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
    int zcount = 3;
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


void World::handlePlayerCommand(bit::RemoteClient &client, Command command)
{
	Player* player = players[client.id];
	sf::Vector2f d;

	switch(command)
	{
		case Command::PlayerUp:
			d.y = -1;
			break;
		case Command::PlayerDown:
			d.y = 1;
			break;
		case Command::PlayerLeft:
			d.x = -1;
			break;
		case Command::PlayerRight:
			d.x = 1;
			break;
	}
	player->zombie->updatePosition(d);
}


sf::Packet& World::compileSnapshot(sf::Packet &packet)
{
    sf::Uint32 zombieCount = zombies.size();
    packet << zombieCount;

    for(unsigned int i=0; i < zombies.size(); i++)
    {
        sf::Uint32 zombieId = i;
        packet << zombieId;
        zombies[i]->compileSnapshot(packet);
    }

    return packet;
}
