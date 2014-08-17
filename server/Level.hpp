#pragma once
#ifndef RZ_LEVEL_H
#define RZ_LEVEL_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "LevelRunner.hpp"
#include "Command.hpp"
#include "Body.hpp"
#include "LevelLoader.hpp"
#include <map>
#include <deque>
#include <functional>

class GameplayServer;
class Character;
class Zombie;
class Ogre;
class Hunter;
class Player;
class Tile;
class Door;
class Chest;
class Wall;
class Light;

class Level
{
public:

    Level();

    ~Level();

    enum State
    {
        Free,
        Combat
    };

    GameplayServer* server;
    unsigned int id;
    State state;
    std::vector<Zombie*> zombies;
    std::vector<Ogre*> ogres;
    std::vector<Hunter*> hunters;
    std::vector<Wall*> walls;
    std::vector<Door*> doors;
    std::vector<Chest*> chests;
    std::vector<Tile*> tiles;
    std::vector<Light*> lights;
	std::map<unsigned int, Player*> players;
    unsigned int tileWidth, tileHeight, tileRows, tileColumns, tileCount, mapWidth, mapHeight;
    std::vector<BaseLevelRunner*> runners;

    Character* characterInTurn;
    std::deque<Character*> turnQueue;

    // Game Logic

    void load(GameplayServer* server, LevelLoader::Level &levelDef);

    void update(sf::Time &gameTime);

    void enterCombat();

    void leaveCombat();

    void endTurn(Character* character);

    DialogNode* getDialogTree();

    
    // Player management

	bool createPlayer(Player* player);

    bool addPlayer(Player* player);

	void removePlayer(Player* player);

	void deletePlayer(Player* player);

    
    // Tile Positioning and Pathfinding

    void iterateTiles(std::function<void(unsigned int index, unsigned int x, unsigned int y, Tile* tile)> inspector);

    Tile* getTileById(unsigned int id);

    Tile* getTileAtIndices(int x, int y);

    bool isCoordinateInMap(float x, float y);

    Tile* getTileAtPosition(float x, float y);

    void getTilesWithinRectangle(float top, float left, float width, float height, std::vector<Tile*> &fill);

    void getShortestPath(float startX, float startY, float endX, float endY, std::function<bool(Tile*)> isBlocked, std::function<void(Tile*, std::vector<Tile*>&)> getNeighbors, std::vector<Tile*> &fill);

    void getCardinalTiles(Tile* tile, std::vector<Tile*> &fill);

    void getCardinalTilesNullsafe(Tile* tile, std::vector<Tile*> &fill, bool nullsafe);

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
        sf::Uint32 id = entity->Body::schema.id;
        packet << id;
        entity->prepareSnapshot(packet, full);
    }
};

#endif