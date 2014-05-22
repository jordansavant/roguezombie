#include "Level.hpp"
#include "Player.hpp"
#include "Tile.hpp"
#include "Light.hpp"
#include "LevelRunner.hpp"
#include "characters/Zombie.hpp"
#include "characters/Ogre.hpp"
#include "characters/Hunter.hpp"
#include "structures/Wall.hpp"
#include "structures/Door.hpp"
#include "structures/Chest.hpp"
#include "levels/Interior.hpp"
#include "items/Item.hpp"
#include "dialog/DialogEntry.hpp"
#include "dialog/DialogNode.hpp"
#include "GameplayServer.hpp"
#include "AiRoutines.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/System.hpp"
#include "../bitengine/Intelligence.hpp"
#include "../bitengine/Intelligence/Shadowcaster.hpp"
#include "../ResourcePath.h"
#include <functional>
#include <sstream>
#include <map>

Level::Level()
    : server(NULL), id(0), state(State::Free), tileWidth(0), tileHeight(0), tileRows(0), tileColumns(0), tileCount(0), characterInTurn(NULL)
{
}

Level::~Level()
{
    for(unsigned int i=0; i < runners.size(); i++)
    {
        delete runners[i];
    }
    turnQueue.clear();
}

/*
 * Game Logic
 */

void Level::load(GameplayServer* _server, unsigned int _id, const int* t_array, int t_rows, int t_cols)
{
    server = _server;
    id = _id;
    state = State::Free;

    // Runners
    runners.push_back(new LevelRunner<Tile>(this, &tiles));
    runners.push_back(new LevelRunner<Zombie>(this, &zombies));
    runners.push_back(new LevelRunner<Ogre>(this, &ogres));
    runners.push_back(new LevelRunner<Hunter>(this, &hunters));
    runners.push_back(new LevelRunner<Wall>(this, &walls));
    runners.push_back(new LevelRunner<Door>(this, &doors));
    runners.push_back(new LevelRunner<Chest>(this, &chests));
    runners.push_back(new LevelRunner<Light>(this, &lights));

    // Map
    tileRows = t_rows;
    tileColumns = t_cols;
    tileCount = tileRows * tileColumns;
    tileWidth = 32;
    tileHeight = 32;
    mapWidth = tileWidth * tileColumns;
    mapHeight = tileHeight * tileRows;
    tiles.resize(tileCount, NULL);

    // Load Tiles
    for (unsigned int j = 0; j < tileRows; ++j)
    {
        for (unsigned int i = 0; i < tileColumns; ++i)
        {
            // get the current tile information
            int index = i + j * tileColumns;
            Tile::Type tileType = Tile::Type::Ground;

            // build the quad and its position on the map
            float originX = i * tileWidth;
            float originY = j * tileHeight;

            // Load our tile
            Tile* t = new Tile();
            t->load(this, index, tileType, originX, originY, tileWidth, tileHeight);
            tiles[index] = t;
        }
    }

    // Load entities
    for (unsigned int j = 0; j < tileRows; ++j)
    {
        for (unsigned int i = 0; i < tileColumns; ++i)
        {
            int index = i + j * tileColumns;
            int logic = t_array[index];
            Tile* t = tiles[index];

            switch(logic)
            {
                case Interior::Spawn::Wall:
                {
                    Wall* wall = new Wall();
                    wall->load(this, server->getNextBodyId(), t->schema.x, t->schema.y);
                    walls.push_back(wall);
                    break;
                }
                case Interior::Spawn::Zombie:
                {
                    Zombie* zombie = new Zombie();
                    zombie->load(this, server->getNextBodyId(), t->schema.x, t->schema.y);
                    zombies.push_back(zombie);
                    zombie->combatDecisionAi = AiRoutines::Combat::DecideZombie;
                    break;
                }
                case Interior::Spawn::Ogre:
                {
                    Ogre* ogre = new Ogre();
                    ogre->load(this, server->getNextBodyId(), t->schema.x, t->schema.y);
                    ogres.push_back(ogre);
                    break;
                }
                case Interior::Spawn::Hunter:
                {
                    Hunter* hunter = new Hunter();
                    hunter->load(this, server->getNextBodyId(), t->schema.x, t->schema.y);
                    hunters.push_back(hunter);

                    hunter->getStartingDialog = std::bind(&Level::getDialogTree, this);
                    break;
                }
                case Interior::Spawn::Light:
                {
                    sf::Color c = sf::Color::Red;
                    Light* light = new Light();
                    light->load(this, t->schema.x, t->schema.y, 4, c, .66);
                    lights.push_back(light);
                    break;
                }
                case Interior::Spawn::Door:
                {
                    Door* door = new Door();
                    door->load(this, server->getNextBodyId(), t->schema.x, t->schema.y);
                    doors.push_back(door);
                    break;
                }
                case Interior::Spawn::Chest:
                {
                    Chest* chest = new Chest();
                    chest->load(this, server->getNextBodyId(), t->schema.x, t->schema.y);
                    chest->addItemToInventory(Item::create(Item::Type::HardHat, server->getNextItemId()));
                    chests.push_back(chest);

                    break;
                }
                case Interior::Spawn::PortalTo2:
                {
                    Level* l = this;
                    t->onBodyEnter.push_back([l] (Tile* t, Body* b){
                        if(b->schema.type == Body::Type::Character)
                        {
                            Character* c = static_cast<Character*>(b);
                            if(c->schema.isPlayerCharacter)
                            {
                                Player* p = l->players[c->schema.clientId];
                                l->server->movePlayerToLevel(p, l->id, 1);
                            }
                        }
                    });
                    break;
                }
                case Interior::Spawn::PortalTo1:
                {
                    Level* l = this;
                    t->onBodyEnter.push_back([l] (Tile* t, Body* b){
                        if(b->schema.type == Body::Type::Character)
                        {
                            Character* c = static_cast<Character*>(b);
                            if(c->schema.isPlayerCharacter)
                            {
                                Player* p = l->players[c->schema.clientId];
                                l->server->movePlayerToLevel(p, l->id, 0);
                            }
                        }
                    });
                    break;
                }
            }
        }
    }
}

void Level::update(sf::Time &gameTime)
{
    // Update entities
    for(unsigned int i=0; i < runners.size(); i++)
    {
        runners[i]->update(gameTime);
    }
    
    switch(state)
    {
        //  If we are in combat mode check that we leave it
        case State::Combat:
            bool endCombat = true;
            for(unsigned int i=0; i < turnQueue.size(); i++)
            {
                Character* c = turnQueue[i];
                if(!c->schema.isDead() && c->isHostileCombatEngaged)
                {
                    endCombat = false;
                }
            }
            if(endCombat)
            {
                leaveCombat();
            }
            break;
    }
}

void Level::enterCombat()
{
    state = State::Combat;

    // Move every character into the turn queue
    for(unsigned int i=0; i < zombies.size(); i++)
        if(!zombies[i]->schema.isDead())
            turnQueue.push_back(zombies[i]);
    for(unsigned int i=0; i < ogres.size(); i++)
        if(!ogres[i]->schema.isDead())
            turnQueue.push_back(ogres[i]);
    for(unsigned int i=0; i < hunters.size(); i++)
        if(!hunters[i]->schema.isDead())
            turnQueue.push_back(hunters[i]);
}

void Level::leaveCombat()
{
    state = State::Free;

    turnQueue.clear();
}

void Level::endTurn(Character* character)
{
    Character* c = turnQueue.front();
    if(c == character)
    {
	    turnQueue.pop_front();
	    turnQueue.push_back(c);
    }
}

DialogNode* Level::getDialogTree()
{
    Level* l = this;

    DialogNode* e = new DialogNode;
    e->prompt = Dialog(server->getNextDialogId(), DialogEntry::Entry::Speaker_Bye);

    DialogNode* d = new DialogNode;
    d->prompt = Dialog(server->getNextDialogId(), DialogEntry::Entry::Speaker_ThatsUnfortunate);
    d->addResponse(Dialog(server->getNextDialogId(), DialogEntry::Entry::Listener_Yes), e);

    DialogNode* c = new DialogNode;
    c->prompt = Dialog(server->getNextDialogId(), DialogEntry::Entry::Speaker_ThatsFortunate);
    c->addResponse(Dialog(server->getNextDialogId(), DialogEntry::Entry::Listener_Yes), e);

    DialogNode* b = new DialogNode;
    b->prompt = Dialog(server->getNextDialogId(), DialogEntry::Entry::Speaker_HowAreYou);
    b->addResponse(Dialog(server->getNextDialogId(), DialogEntry::Entry::Listener_IAmGood), c);
    b->addResponse(Dialog(server->getNextDialogId(), DialogEntry::Entry::Listener_IAmBad), d, NULL, [l] (Body* speaker, Body* listener){
        listener->addItemToInventory(Item::create(Item::Type::Magnum357, l->server->getNextItemId()));
    });

    DialogNode* a = new DialogNode;
    a->prompt = Dialog(server->getNextDialogId(), DialogEntry::Entry::Speaker_Hello);
    a->addResponse(Dialog(server->getNextDialogId(), DialogEntry::Entry::Listener_Hello), b);

    return a;
}

/*
 * Player Management
 */

bool Level::createPlayer(Player* player)
{
    // Create character
    Zombie* zombie = new Zombie();
    zombie->load(this, server->getNextBodyId(), 0, 0);

    // Assign Character
    player->setCharacter(zombie);
    zombie->setControllingPlayer(player);

    // Create light source for character's vision
    Light* light = new Light();
    light->load(this, zombie->Body::schema.x, zombie->Body::schema.y, 12, sf::Color(220, 255, 175), .4);
    zombie->lights.push_back(light);
    Light* orbLight = new Light();
    orbLight->load(this, zombie->Body::schema.x, zombie->Body::schema.y, 4, sf::Color::White, .6);
    zombie->lights.push_back(orbLight);

    // Add player to this level's management
    if(!addPlayer(player))
    {
        deletePlayer(player);

        return false;
    }

    return true;
}

bool Level::addPlayer(Player* player)
{
    // Connect to Level
    player->setLevel(this);
    players[player->clientId] = player;

    // Push character into management list
    player->character->level = this;
    switch(player->character->schema.type)
    {
        case Character::Type::Zombie:
            zombies.push_back(static_cast<Zombie*>(player->character));
            break;
        case Character::Type::Ogre:
            ogres.push_back(static_cast<Ogre*>(player->character));
            break;
    }

    // Manage lights for the character
    for(unsigned int i=0; i < player->character->lights.size(); i++)
    {
        player->character->lights[i]->level = this;
        lights.push_back(player->character->lights[i]);
    }

    // Add body to tiles
    if(!player->character->moveToPosition(tiles[44]->schema.x, tiles[44]->schema.y))
    {
        removePlayer(player);

        return false;
    }

    return true;
}

void Level::removePlayer(Player* player)
{
    if(players.find(player->clientId) != players.end())
	{
        // Disconnect from Level
        player->setLevel(NULL);
        players.erase(player->clientId);

        // Remove body from tiles
        std::vector<Tile*> tiles;
        getTilesWithinRectangle(player->character->Body::schema.x, player->character->Body::schema.y, player->character->Body::schema.width, player->character->Body::schema.height, tiles);
        for(unsigned int i=0; i < tiles.size(); i++)
        {
            if(tiles[i]->body && tiles[i]->body == player->character)
            {
                tiles[i]->setOccupyingBody(NULL);
            }
        }

        // Remove lights from management list
        for(unsigned int i=0; i < player->character->lights.size(); i++)
        {
            lights.erase(std::remove(lights.begin(), lights.end(), player->character->lights[i]), lights.end());
        }

        // Remove character from management list
        switch(player->character->schema.type)
        {
            case Character::Type::Zombie:
                zombies.erase(std::remove(zombies.begin(), zombies.end(), static_cast<Zombie*>(player->character)), zombies.end());
                break;
            case Character::Type::Ogre:
                ogres.erase(std::remove(ogres.begin(), ogres.end(), static_cast<Ogre*>(player->character)), ogres.end());
                break;
        }
	}
}

void Level::deletePlayer(Player* player)
{
    // Remove player
    removePlayer(player);

    // Delete light source for character's vision
    for(unsigned int i=0; i < player->character->lights.size(); i++)
    {
        delete player->character->lights[i];
    }

    // Delete character
    delete player->character;

    // Delete player
    delete player;
}

/*
 * Tile Positioning and Pathfinding
 */

Tile* Level::getTileById(unsigned int id)
{
    if(id < tiles.size())
        return tiles[id];
    return NULL;
}

Tile* Level::getTileAtIndices(int x, int y)
{
    unsigned int index = x + (tileColumns * y);

    if(index < tiles.size())
        return tiles[index];
    return NULL;
}

bool Level::isCoordinateInMap(float x, float y)
{
    if(x >= mapWidth || x < 0)
        return false;
    if(y >= mapHeight || y < 0)
        return false;

    return true;
}

Tile* Level::getTileAtPosition(float x, float y)
{
    if(!isCoordinateInMap(x, y))
        return NULL;

    unsigned int tx = (unsigned int)std::floor((float)x / (float)tileWidth);
    unsigned int ty = (unsigned int)std::floor((float)y / (float)tileHeight);
    unsigned int index = tx + (tileColumns * ty);

    return tiles[index];
}

void Level::getTilesWithinRectangle(float left, float top, float width, float height, std::vector<Tile*> &fill)
{
    // Get the total size of the tiles
    int recTileWidth = (int)width / tileWidth;
    int recTileHeight = (int)height / tileHeight;
    int totalTiles = recTileWidth * recTileHeight;

    for(int x = left; x < left + width; x += tileWidth)
    {
        for(int y = top; y < top + height; y += tileHeight)
        {
            Tile* t = getTileAtPosition(x, y);
            if(t)
            {
                fill.push_back(t);
            }
        }
    }
}

void Level::getShortestPath(float startX, float startY, float endX, float endY, std::function<bool(Tile*)> isBlocked, std::function<void(Tile*, std::vector<Tile*>&)> getNeighbors, std::vector<Tile*> &fill)
{
    // Translate start and finish to tiles
    Tile* closestStartNode = getTileAtPosition(startX, startY);
    Tile* closestFinishNode = getTileAtPosition(endX, endY);

    // Return empty list if there is no path
    if (closestStartNode == NULL || closestFinishNode == NULL || isBlocked(closestStartNode) || isBlocked(closestFinishNode))
        return;

    bit::Astar::pathfind(closestStartNode, closestFinishNode, isBlocked, getNeighbors, fill);
}

void Level::getCardinalTiles(Tile* tile, std::vector<Tile*> &fill)
{
    getCardinalTilesNullsafe(tile, fill, true);
}

void Level::getCardinalTilesNullsafe(Tile* tile, std::vector<Tile*> &fill, bool nullsafe)
{
    Tile* top = getTileAtPosition(tile->schema.x, tile->schema.y - tile->schema.height);
    Tile* bottom = getTileAtPosition(tile->schema.x, tile->schema.y + tile->schema.height);
    Tile* left = getTileAtPosition(tile->schema.x - tile->schema.width, tile->schema.y);
    Tile* right = getTileAtPosition(tile->schema.x + tile->schema.width, tile->schema.y);

    if(top)
        fill.push_back(top);
    else if(!nullsafe)
        fill.push_back(top);

    if(bottom)
        fill.push_back(bottom);
    else if(!nullsafe)
        fill.push_back(bottom);

    if(left)
        fill.push_back(left);
    else if(!nullsafe)
        fill.push_back(left);

    if(right)
        fill.push_back(right);
    else if(!nullsafe)
        fill.push_back(right);
}

void Level::raycastTiles(float startX, float startY, float endX, float endY, std::function<bool(Tile*)> inspect)
{
    // Convert to tile coordinates
    unsigned int stx = (unsigned int)std::floor((float)startX / (float)tileWidth);
    unsigned int sty = (unsigned int)std::floor((float)startY / (float)tileHeight);
    unsigned int etx = (unsigned int)std::floor((float)endX / (float)tileWidth);
    unsigned int ety = (unsigned int)std::floor((float)endY / (float)tileHeight);

    std::vector<sf::Vector2i> line = bit::VectorMath::bresenhamLine(stx, sty, etx, ety);

    for(unsigned int i=0; i < line.size(); i++)
    {
        unsigned int index = line[i].x + line[i].y * tileColumns;
        if(index < tiles.size())
        {
            Tile* t = tiles[index];

            if(inspect(t))
            {
                return;
            }
        }
    }
}

/*
 * Networking
 */

void Level::handlePlayerCommand(bit::ClientPacket &packet, bit::RemoteClient &client, Command::Type commandType)
{
	Player* player = players[client.id];
    player->handleCommand(packet, commandType);
}

void Level::prepareSnapshot(bit::ServerPacket &packet, bit::RemoteClient& client, bool full)
{
    Player* p = players[client.id];

    // Current level state
    packet << sf::Uint32(state);

    // Get a subset of visible tiles for the player within a radius of tiles
    std::vector<Tile*> visibles;
    p->character->inspectVisibleTiles([&visibles] (Tile* t) {
        visibles.push_back(t);
    });

    // indicate number of tiles
    packet << sf::Uint32(visibles.size());

    for(unsigned int i=0; i < visibles.size(); i++)
    {
        Tile* t = visibles[i];

        // shove a tile type and tile info on there
        sf::Uint32 tileId = t->schema.id;
        packet << tileId;
        t->prepareSnapshot(packet, full);

        Body* b;
        // If no body or door, say no body
        if(!t->body && !t->door)
        {
            packet << sf::Uint32(Body::Type::None);
            continue;
        }
        // If there is a body, use it
        else if(t->body)
        {
            b = t->body;
        }
        // If there is a door, use it
        else
        {
            b = t->door;
        }

        // break open body
        switch(b->schema.type)
        {
            case Body::Type::Character:
            {
                Character* c = static_cast<Character*>(b);
                switch(c->schema.type)
                {
                    case Character::Type::Zombie:
                        packNetworkBody<Zombie, Character>(packet, full, c, b->schema.type, c->schema.type);
                        break;
                    case Character::Type::Ogre:
                        packNetworkBody<Ogre, Character>(packet, full, c, b->schema.type, c->schema.type);
                        break;
                    case Character::Type::Hunter:
                        packNetworkBody<Hunter, Character>(packet, full, c, b->schema.type, c->schema.type);
                        break;
                }
                break;
            }

            case Body::Type::Structure:
            {
                Structure* s = static_cast<Structure*>(b);
                switch(s->schema.type)
                {
                    case Structure::Type::Wall:
                        packNetworkBody<Wall, Structure>(packet, full, s, b->schema.type, s->schema.type);
                        break;
                    case Structure::Type::Door:
                        packNetworkBody<Door, Structure>(packet, full, s, b->schema.type, s->schema.type);
                        break;
                    case Structure::Type::Chest:
                        packNetworkBody<Chest, Structure>(packet, full, s, b->schema.type, s->schema.type);
                        break;
                }
                break;
            }
            case Body::Type::None:
                break;
        }
    }
}