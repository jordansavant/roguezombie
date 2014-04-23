#include "Character.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "GameEvent.hpp"
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
    : Body(), moveTimer(.75f), fixedState(), deltaState(), backpack(NULL), backpackClient()
{
}

Character::~Character()
{
    for(unsigned int i=0; i < missions.size(); i++)
    {
        delete missions[i];
    }

    delete backpack;
}

void Character::load(Level* _level, unsigned int _id, Type _type, float _x, float _y, float _width, float _height)
{
    Body::load(_level, _id, Body::Type::Character, _x, _y, _width, _height);
    fixedState.type = _type;
    fixedState.maxHealth = 100;
	deltaState.health = 100;

    backpack = Item::create(Item::Type::Backpack);
    backpack->parentCharacter = this;
    backpack->id = level->server->getNextItemId();

    moveToPosition(_x, _y);
}

void Character::update(sf::Time &gameTime)
{
    Body::update(gameTime);

    // Follow designation path points
    if(path.size() > 0 && moveTimer.update(gameTime))
    {
        Tile* nextTile = path.back();

        if(nextTile->fixedState.x != Body::schema.x || nextTile->fixedState.y != Body::schema.y)
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
            lights[i]->x = Body::schema.x;
            lights[i]->y = Body::schema.y;
        }
    }

    // See if any missions are complete // TODO: distribute into a less updated manner
    checkMissions();
}

void Character::setControllingPlayer(Player* player)
{
    fixedState.isPlayerCharacter = true;
    fixedState.clientId = player->clientId;
    fixedState.player = player;
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
    return moveToPosition(t->fixedState.x, t->fixedState.y);
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
    level->getTilesWithinRectangle(tile->fixedState.x, tile->fixedState.y, Body::schema.width, Body::schema.height, tiles);
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

    if(fixedState.isPlayerCharacter)
    {
        Character* c = this;/*
        level->server->sendEventToClient(*fixedState.player->client, [mission] (bit::ServerPacket &packet) {
            packet << sf::Uint32(GameEvent::MissionAssigned);
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

void Character::addItemToInventory(Item* item)
{
    backpack->addItem(item);
}

void Character::handleMissionCompleteGameEvent(bit::ServerPacket &packet)
{
    unsigned int depth;
    packet >> depth;

    MissionClient* mc = NULL;
    for(unsigned int i=0; i < depth; i++)
    {
        unsigned int missionId;
        packet >> missionId;

        if(!mc)
            mc = &missionClients[missionId];
        else
            mc = &mc->childMissions[missionId];
    }

    if(mc)
    {
        mc->isComplete = true;
    }
}

void Character::handleItemAddGameEvent(bit::ServerPacket &packet)
{
    // depth includes all parents and the new item id
    unsigned int depth;
    packet >> depth;

    unsigned int backpack_id;
    packet >> backpack_id;
    ItemClient* ic = &backpackClient;
    // Adding an item to the inventory normally
    // skip the first and last ID as that is our backpack then new item
    for(unsigned int i=1; i < depth - 1; i++)
    {
        unsigned int itemId;
        packet >> itemId;
        ic = &backpackClient.itemClients[itemId];
    }

    if(ic)
    {
        unsigned int itemId;
        packet >> itemId;

        // unpack the item into it
        backpackClient.itemClients[itemId] = ItemClient();
        backpackClient.itemClients[itemId].handleSnapshot(packet);
    }
}

void Character::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    // Body
    Body::prepareSnapshot(packet, full);

    // Character
    packet << fixedState;
    packet << deltaState;

    // Missions
    packet << full;
    if(full)
    {
        // If we need to update missions
        packet << sf::Uint32(missions.size());
        for(unsigned int i=0; i < missions.size(); i++)
        {
            packet << sf::Uint32(missions[i]->id);
            missions[i]->prepareSnapshot(packet);
        }
    }

    // Items
    packet << full;
    if(full)
    {
        backpack->prepareSnapshot(packet);
    }
}

void Character::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    // Body
    Body::handleSnapshot(packet, full);

    // Character
    packet >> fixedState;
    packet >> deltaState;

    // Mission Clientside
    bool hasMissionUpdate;
    packet >> hasMissionUpdate;
    if(hasMissionUpdate)
    {
        unsigned int missionClientSize;
        packet >> missionClientSize;
        if(missionClientSize > 0)
        {
            missionClients.clear();
            for(unsigned int i=0; i < missionClientSize; i++)
            {
                unsigned int missionId;
                packet >> missionId;
                missionClients[missionId] = MissionClient();
                missionClients[missionId].handleSnapshot(packet);
            }
        }
    }

    // Item Clientside
    bool hasBackpack;
    packet >> hasBackpack;
    if(hasBackpack)
    {
        backpackClient.handleSnapshot(packet);
    }
}