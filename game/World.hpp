#pragma once
#ifndef RZ_WORLD_H
#define RZ_WORLD_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "Command.hpp"
#include <map>

namespace bit
{
	class RemoteClient;
    class ServerPacket;
    class ClientPacket;
}

class Zombie;
class Ogre;
class Player;
class Tile;
class Wall;

class World
{
public:

    World();

    ~World();

    std::vector<Zombie*> zombies;
    std::vector<Ogre*> ogres;
    std::vector<Wall*> walls;
    std::vector<Tile*> tiles;
	std::map<unsigned int, Player*> players;
    unsigned int tileWidth, tileHeight, tileRows, tileColumns, tileCount, mapWidth, mapHeight;

    void load();

    void update(sf::Time &gameTime);

	void createPlayer(bit::RemoteClient &client);

    void handlePlayerCommand(bit::ClientPacket &packet, bit::RemoteClient &client, Command::Type command);

    bool isCoordinateInMap(float x, float y);

    Tile* getTileAtPosition(float x, float y);

    std::vector<Tile*> getTilesWithinRectangle(float top, float left, float width, float height);

    void prepareSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif