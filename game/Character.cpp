#include "Character.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "ServerEvent.hpp"
#include "GameplayServer.hpp"
#include "Level.hpp"
#include "Body.hpp"
#include "Tile.hpp"
#include "Light.hpp"
#include "Player.hpp"
#include "mission/Mission.hpp"
#include "mission/MissionClient.hpp"
#include "items/Item.hpp"

Character::Character()
    : Body(), moveTimer(.75f), schema()
{
}

Character::~Character()
{
    for(unsigned int i=0; i < missions.size(); i++)
    {
        delete missions[i];
    }
}

void Character::load(Level* _level, unsigned int _id, Type _type, float _x, float _y, float _width, float _height)
{
    Body::load(_level, _id, Body::Type::Character, _x, _y, _width, _height);
    blockFoV = false;
    schema.type = _type;
    schema.maxHealth = 100;
	schema.health = 100;

    moveToPosition(_x, _y);
}

void Character::update(sf::Time &gameTime)
{
    Body::update(gameTime);

    // Follow designation path points
    if(path.size() > 0 && moveTimer.update(gameTime))
    {
        Tile* nextTile = path.back();

        if(nextTile->schema.x != Body::schema.x || nextTile->schema.y != Body::schema.y)
        {
            if(moveToTile(nextTile))
            {
                path.pop_back();
            }
        }
        else
        {
            path.pop_back();
        }
    }

    // If I am a player character and I have a light, update it
    if(schema.isPlayerCharacter)
    {
        for(unsigned int i=0; i < lights.size(); i++)
        {
            lights[i]->x = Body::schema.x;
            lights[i]->y = Body::schema.y;
        }
    }

    // See if any missions are complete // TODO: distribute into a less updated manner
    checkMissions();
}

void Character::setControllingPlayer(Player* player)
{
    schema.isPlayerCharacter = true;
    schema.clientId = player->clientId;
    schema.player = player;
}

bool Character::moveUp()
{
    return moveToPosition(Body::schema.x, Body::schema.y - level->tileHeight);
}

bool Character::moveDown()
{
    return moveToPosition(Body::schema.x, Body::schema.y + level->tileHeight);
}

bool Character::moveLeft()
{
    return moveToPosition(Body::schema.x - level->tileWidth, Body::schema.y);
}

bool Character::moveRight()
{
    return moveToPosition(Body::schema.x + level->tileWidth, Body::schema.y);
}

bool Character::moveToTile(Tile* t)
{
    return moveToPosition(t->schema.x, t->schema.y);
}

bool Character::moveToPosition(float x, float y)
{
    std::vector<Tile*> newTiles;
    level->getTilesWithinRectangle(x, y, Body::schema.width, Body::schema.height, newTiles);
    std::vector<Tile*> currentTiles;
    level->getTilesWithinRectangle(Body::schema.x, Body::schema.y, Body::schema.width, Body::schema.height, currentTiles);

    // Check if I can move
    bool canMove = true;
    for(unsigned int i=0; i < newTiles.size(); i++)
    {
        if(isTileBlocked(newTiles[i]))
        {
            canMove = false;
            break;
        }
    }
    if(canMove)
    {
        for(unsigned int i=0; i < currentTiles.size(); i++)
        {
            currentTiles[i]->setOccupyingBody(NULL);
        }
        for(unsigned int i=0; i < newTiles.size(); i++)
        {
            newTiles[i]->setOccupyingBody(this);
        }

        Body::schema.x = x;
        Body::schema.y = y;
    }

    return canMove;
}

void Character::pathToPosition(float x, float y)
{
    path.clear();
    level->getShortestPath(
        Body::schema.x,
        Body::schema.y,
        x,
        y,
        std::bind(
            &Character::isTileBlockedForPathfinding,
            this,
            std::placeholders::_1),
        std::bind(
            &Level::getCardinalTiles,
            level,
            std::placeholders::_1,
            std::placeholders::_2),
        path);
}

bool Character::isTileBlocked(Tile* tile)
{
    return (!tile || (tile->body && tile->body != this));
}

bool Character::isTileBlockedForPathfinding(Tile* tile)
{
    // Look at all tiles within my width and height
    std::vector<Tile*> tiles;
    level->getTilesWithinRectangle(tile->schema.x, tile->schema.y, Body::schema.width, Body::schema.height, tiles);
    for(unsigned int i=0; i < tiles.size(); i++)
    {
        if(isTileBlocked(tiles[i]))
        {
            return true;
        }
    }

    return false;
}

void Character::assignMission(Mission* mission)
{
    missions.push_back(mission);
    mission->parentCharacter = this;

    if(schema.isPlayerCharacter)
    {
        Character* c = this;/*
        level->server->sendEventToClient(*fixedState.player->client, [mission] (bit::ServerPacket &packet) {
            packet << sf::Uint32(ServerEvent::MissionAssigned);
        });*/
    }
}

void Character::checkMissions()
{
    for(unsigned int i=0; i < missions.size(); i++)
    {
        missions[i]->attemptCompleteMission();
    }
}

void Character::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    // Body
    Body::prepareSnapshot(packet, full);

    // Character
    packet << schema;

    // Missions
    packet << full;
    if(full)
    {
        // If we need to update missions
        packet << sf::Uint32(missions.size());
        for(unsigned int i=0; i < missions.size(); i++)
        {
            packet << sf::Uint32(missions[i]->schema.id);
            missions[i]->prepareSnapshot(packet);
        }
    }
}