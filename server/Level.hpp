#pragma once
#ifndef RZ_LEVEL_H
#define RZ_LEVEL_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Structures.hpp"
#include "LevelRunner.hpp"
#include "Command.hpp"
#include "Body.hpp"
#include "LevelLoader.hpp"
#include <map>
#include <deque>
#include <functional>

class GameplayServer;
class Character;
class Structure;
class Zombie;
class Ogre;
class Hunter;
class Scientist;
class Guard;
class Hazmaster;
class Batman;
class Skeleton;
class SkeletonLord;
class Player;
class Tile;
class Door;
class Chest;
class Furnishing;
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
    unsigned int defaultEntranceId;
    State state;
    std::vector<Character*> characters; // meta list
    std::vector<Zombie*> zombies;
    std::vector<Ogre*> ogres;
    std::vector<Hunter*> hunters;
    std::vector<Scientist*> scientists;
    std::vector<Guard*> guards;
    std::vector<Hazmaster*> hazmasters;
    std::vector<Batman*> batmans;
    std::vector<Skeleton*> skeletons;
    std::vector<SkeletonLord*> skeletonLords;
    std::vector<Structure*> structures; // meta list
    std::vector<Wall*> walls;
    std::vector<Door*> doors;
    std::vector<Chest*> chests;
    std::vector<Furnishing*> furnishings;
    std::vector<Tile*> tiles;
    std::vector<Light*> lights;
    bit::QuadTree<Tile>* tileQuadTree;
    std::map<unsigned int, Player*> players;
    unsigned int tileWidth, tileHeight; // size of individual tile e.g. 32, 32
    unsigned int tileRows, tileColumns, tileCount; // number of tiles in units e.g. 100, 100, 10000
    unsigned int mapWidth, mapHeight; // size of map in tile dimesions 3200, 3200
    std::vector<BaseLevelRunner*> runners;
    unsigned int metadata_playerNearbyUpdateId;

    Character* characterInTurn;
    std::deque<Character*> turnQueue;

    // Game Logic

    void load(GameplayServer* server, LevelLoader::Level &levelDef);

    void loadEventIntoTile(bit::Event<std::function<void(Tile* t, Body* body)>> &e, LevelLoader::Event &eventDef);

    void update(sf::Time &gameTime);

    void enterCombat();

    void leaveCombat();

    void endTurn(Character* character);

    DialogNode* getDialogTree();

    void endGameVictory();

    void endGameDefeat();

    void removeBody(Body* body);

    void removeCharacter(Character* character);

    void removeStructure(Structure* structure);

	void dev_killAll();

    
    // Player management

    bool createPlayer(Player* player);

    bool canAddPlayer(Player* player, unsigned int entranceId);

    bool addPlayer(Player* player, unsigned int entranceId);

    void removePlayer(Player* player);

    void deletePlayer(Player* player);

    bool canSetCharacterAtEntrance(Character* character, unsigned int entranceId);

    bool setCharacterAtEntrance(Character* character, unsigned int entranceId);

    void movePlayerToLevel(Player* player, unsigned int toLevelId, unsigned int toEntranceId);

    bool isWithinRangeOfPlayer(float x, float y, float rangeCheck);
    

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

    Tile* getAvailableEntranceTile(unsigned int entranceId);


    // Character and Structure helpers

    void iterateCharacters(std::function<void(Character* character)> inspector);

    void createLightFlare(float x, float y, float seconds, sf::Color &startColor, sf::Color &endColor, float startRadius, float endRadius, float startBrightness, float endBrightness = 0);

    Character* getCharacterByPosition(float x, float y);


    // Networking

    void sendEventToAllPlayers(std::function<void(bit::ServerPacket&)> prepare);

    void sendEventToPlayer(Player* player, std::function<void(bit::ServerPacket&)> prepare);

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