#pragma once
#ifndef RZ_WORLD_H
#define RZ_WORLD_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "Command.hpp"
#include <map>
#include <functional>

class Zombie;
class Ogre;
class Player;
class Tile;
class Wall;
class Light;

class World
{
public:

    World();

    ~World();

    std::vector<Zombie*> zombies;
    std::vector<Ogre*> ogres;
    std::vector<Wall*> walls;
    std::vector<Tile*> tiles;
    std::vector<Light*> lights;
	std::map<unsigned int, Player*> players;
    unsigned int tileWidth, tileHeight, tileRows, tileColumns, tileCount, mapWidth, mapHeight;


    // Game Logic

    void load();

    void update(sf::Time &gameTime);

	void createPlayer(bit::RemoteClient &client);


    // Tile Positioning and Pathfinding

    bool isCoordinateInMap(float x, float y);

    Tile* getTileAtPosition(float x, float y);

    std::vector<Tile*> getTilesWithinRectangle(float top, float left, float width, float height);

    std::vector<Tile*> getShortestPath(float startX, float startY, float endX, float endY, std::function<bool(Tile*)> isBlocked, std::function<std::vector<Tile*>(Tile*)> getNeighbors);

    std::vector<Tile*> getCardinalTiles(Tile* tile);

    std::vector<Tile*> getCardinalTiles(Tile* tile, bool nullsafe);

    void raycastTiles(float startX, float startY, float endX, float endY, std::function<bool(Tile*)> inspect);


    // Field of View

    unsigned int shadowcastGetWidth();

    unsigned int shadowcastGetHeight();

    void shadowcastSetVisible(int x, int y, int distance);

    bool shadowcastIsBlocked(int x, int y);


    // Networking

    void handlePlayerCommand(bit::ClientPacket &packet, bit::RemoteClient &client, Command::Type command);

    void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

};

#endif