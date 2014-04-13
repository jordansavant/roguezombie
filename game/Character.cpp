#include "Character.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "Level.hpp"
#include "Body.hpp"
#include "Tile.hpp"
#include "Light.hpp"
#include "Player.hpp"
#include "mission/Mission.hpp"
#include "mission/MissionClient.hpp"

Character::Character()
    : Body(), missionStateChanged(false), moveTimer(.75f), fixedState(), deltaState()
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
    fixedState.type = _type;
    fixedState.maxHealth = 100;
	deltaState.health = 100;

    moveToPosition(_x, _y);
}

void Character::update(sf::Time &gameTime)
{
    Body::update(gameTime);

    // Follow designation path points
    if(path.size() > 0 && moveTimer.update(gameTime))
    {
        Tile* nextTile = path.back();

        if(nextTile->fixedState.x != Body::deltaState.x || nextTile->fixedState.y != Body::deltaState.y)
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
    if(fixedState.isPlayerCharacter)
    {
        for(unsigned int i=0; i < lights.size(); i++)
        {
            lights[i]->x = Body::deltaState.x;
            lights[i]->y = Body::deltaState.y;
        }
    }

    // See if any missions are complete // TODO: distribute into a less updated manner
    checkMissions();
}

void Character::setControllingPlayer(Player* player)
{
    fixedState.isPlayerCharacter = true;
    fixedState.clientId = player->clientId;
}

bool Character::moveUp()
{
    return moveToPosition(Body::deltaState.x, Body::deltaState.y - level->tileHeight);
}

bool Character::moveDown()
{
    return moveToPosition(Body::deltaState.x, Body::deltaState.y + level->tileHeight);
}

bool Character::moveLeft()
{
    return moveToPosition(Body::deltaState.x - level->tileWidth, Body::deltaState.y);
}

bool Character::moveRight()
{
    return moveToPosition(Body::deltaState.x + level->tileWidth, Body::deltaState.y);
}

bool Character::moveToTile(Tile* t)
{
    return moveToPosition(t->fixedState.x, t->fixedState.y);
}

bool Character::moveToPosition(float x, float y)
{
    std::vector<Tile*> newTiles;
    level->getTilesWithinRectangle(x, y, Body::deltaState.width, Body::deltaState.height, newTiles);
    std::vector<Tile*> currentTiles;
    level->getTilesWithinRectangle(Body::deltaState.x, Body::deltaState.y, Body::deltaState.width, Body::deltaState.height, currentTiles);

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

        Body::deltaState.x = x;
        Body::deltaState.y = y;
    }

    return canMove;
}

void Character::pathToPosition(float x, float y)
{
    path.clear();
    level->getShortestPath(
        Body::deltaState.x,
        Body::deltaState.y,
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
    level->getTilesWithinRectangle(tile->fixedState.x, tile->fixedState.y, Body::deltaState.width, Body::deltaState.height, tiles);
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
    missionStateChanged = true;
    mission->parentCharacter = this;
}

void Character::checkMissions()
{
    for(unsigned int i=0; i < missions.size(); i++)
    {
        if(missions[i]->attemptCompleteMission())
        {
        }
    }
}

void Character::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    // Body
    Body::prepareSnapshot(packet, full);

    // Character
    if(full)
        packet << fixedState;
    packet << deltaState;

    // Missions
    if(missionStateChanged)
    {
        // If we need to update missions
        packet << sf::Uint32(missions.size());
        for(unsigned int i=0; i < missions.size(); i++)
        {
            missions[i]->prepareSnapshot(packet);
        }
        missionStateChanged = false;
    }
    else
    {
        // If we do not need to update missions
        packet << sf::Uint32(0);
    }
}

void Character::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    // Body
    Body::handleSnapshot(packet, full);

    // Character
    if(full)
        packet >> fixedState;
    packet >> deltaState;

    // Mission Clientside
    unsigned int missionClientSize;
    packet >> missionClientSize;
    if(missionClientSize > 0)
    {
        missionClients.resize(missionClientSize, MissionClient());
        for(unsigned int i=0; i < missionClients.size(); i++)
        {
            missionClients[i].handleSnapshot(packet);
        }
    }
}
