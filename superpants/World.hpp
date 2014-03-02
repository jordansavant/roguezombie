#pragma once
#ifndef RZ_WORLD_H
#define RZ_WORLD_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include <map>
#include "Command.hpp"

namespace bit
{
	class RemoteClient;
    class ServerPacket;
    class ClientPacket;
}

class Zombie;
class Player;

class World
{
public:

    World();

    ~World();

    std::vector<Zombie*> zombies;
	std::map<unsigned int, Player*> players;

    void load();

    void update(sf::Time &gameTime);

	void createPlayer(bit::RemoteClient &client);

    void handlePlayerCommand(bit::ClientPacket &packet, bit::RemoteClient &client, Command::Type command);

    void prepareSnapshot(bit::ServerPacket &packet);
};

#endif