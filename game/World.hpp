#pragma once
#ifndef RZ_WORLD_H
#define RZ_WORLD_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Structures.hpp"
#include "WorldRunner.hpp"
#include "Command.hpp"
#include <map>
#include <unordered_map>
#include <functional>

class Zombie;
class Ogre;
class Player;
class Tile;
class Door;
class Wall;
class Light;
class Chunk;

class World
{
public:

    World();

    ~World();

    std::vector<Zombie*> zombies;
    std::vector<Ogre*> ogres;
    std::vector<Wall*> walls;
    std::vector<Door*> doors;
    std::vector<Tile*> tiles;
    std::vector<Light*> lights;
	std::map<unsigned int, Player*> players;
    unsigned int tileWidth, tileHeight, tileRows, tileColumns, mapWidth, mapHeight;

    unsigned int tileIdCounter;

    std::unordered_map<int, std::unordered_map<int, Chunk*>> chunks;
    unsigned int chunkWidth, chunkHeight;

    std::vector<BaseWorldRunner*> runners;

    // Game Logic

    void load();

    void update(sf::Time &gameTime);

	void createPlayer(bit::RemoteClient &client);

    // Chunks

    void regionalizeChunks(int x, int y);

    void loadChunkAt(int x, int y);

    Chunk* getChunkAt(int x, int y);

    Tile* getTileAt(int x, int y);

    // Tile Positioning and Pathfinding

    Tile* getTileAtIndices(int x, int y);

    bool isCoordinateInMap(float x, float y);

    Tile* getTileAtPosition(int x, int y);

    void getTilesWithinRectangle(float top, float left, float width, float height, std::vector<Tile*> &fill);

    void getShortestPath(float startX, float startY, float endX, float endY, std::function<bool(Tile*)> isBlocked, std::function<void(Tile*, std::vector<Tile*>&)> getNeighbors, std::vector<Tile*> &fill);

    void getCardinalTiles(Tile* tile, std::vector<Tile*> &fill);

    void getCardinalTiles(Tile* tile, std::vector<Tile*> &fill, bool nullsafe);

    void raycastTiles(float startX, float startY, float endX, float endY, std::function<bool(Tile*)> inspect);

    // Networking

    void handlePlayerCommand(bit::ClientPacket &packet, bit::RemoteClient &client, Command::Type command);

    void prepareSnapshot(bit::ServerPacket &packet, bit::RemoteClient& client, bool full = false);

    template <class T, class C>
    void packNetworkBody(bit::ServerPacket &packet, bool full, C* c, unsigned int bodyType, unsigned int subType)
    {
        packet << sf::Uint32(bodyType);
        packet << sf::Uint32(subType);
        T* entity = static_cast<T*>(c);
        sf::Uint32 id = entity->Body::fixedState.id;
        packet << id;
        entity->prepareSnapshot(packet, full);
    }
};

#endif