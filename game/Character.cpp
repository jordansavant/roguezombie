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
    : Body(), combatState(CombatState::Waiting), combatAction(CombatAction::Move), actionDelayTimer(1), hostilityCheckAi(NULL), combatDetectionAi(NULL), combatDecisionAi(NULL), isHostileCombatDetected(false), targetEnemy(NULL), combatTilesTraversed(0), moveTimer(.67f), equipment(), schema(), visionRadius(30)
{
    equipment.resize(EquipmentSlot::_count, NULL);
}

Character::~Character()
{
    for(unsigned int i=0; i < missions.size(); i++)
    {
        delete missions[i];
    }

    for(unsigned int i=0; i < equipment.size(); i++)
    {
        delete equipment[i];
    }
}

bool Character::Schema::isDead()
{
    return health <= 0;
}

///////////////////////////////////////////////////////
//                  GAME LOOP                        //
///////////////////////////////////////////////////////

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

    if(!schema.isDead())
    {
        updateAlive(gameTime);
    }
    else
    {
        updateDead(gameTime);
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

void Character::updateAlive(sf::Time &gameTime)
{
    switch(level->state)
    {
        case Level::State::Free:
            updateAlive_Free(gameTime);
            break;

        case Level::State::Combat:
            updateAlive_Combat(gameTime);
            break;
    }
}

void Character::updateAlive_Free(sf::Time &gameTime)
{
    // TODO: THIS NEEDS TO BE DISTRIBUTED FOR PERFORMANCE!!!
    // Check for combatable scenario to begin
    detectCombatEnter();

    followPath(gameTime);
}

void Character::updateAlive_Combat(sf::Time &gameTime)
{
    switch(combatState)
    {
        case CombatState::Waiting:

            // Its my turn
            if(this == level->turnQueue.front())
            {
                // Refill my action points and move to decision making
                schema.currentActionPoints = schema.maxActionPoints;
                combatState = CombatState::DecideAction;
            }

            break;

        case CombatState::DecideAction:

            // Check for combatable scenario to end
            detectCombatExit();

            // If I have action points remaining
            if(schema.currentActionPoints > 0)
			{
                // Make a decision in combat
                combat_DecideAction(gameTime);
			}
			else
			{
                // If I am out of action points, then resume waiting my turn and signal my turn is over
				level->endTurn(this);
                combatState = CombatState::Waiting;
			}
            break;

        case CombatState::PerformAction:

            switch(combatAction)
            {
                case CombatAction::Move:

                    combat_PerformAction_MoveToLocation(gameTime);

                    break;

                case CombatAction::Attack:

                    combat_PerformAction_AttackCharacter(gameTime);

                    break;
            }

            break;

        case CombatState::Delay:

            // Allows us to wait before next action point usage for delay between actions
            // Helps the simulation feel natural
            if(actionDelayTimer.update(gameTime))
            {
                combatState = CombatState::DecideAction;
            }

            break;
    }
}

void Character::updateDead(sf::Time &gameTime)
{
    switch(level->state)
    {
        case Level::State::Free:
            break;

        case Level::State::Combat:
            level->endTurn(this);
            break;
    }
}

void Character::combat_DecideAction(sf::Time &gameTime)
{
    if(combatDecisionAi)
    {
        combatDecisionAi(this);
    }
    else if(schema.isPlayerCharacter)
    {
        // Nil, remain in this state until player command is received
    }
    else
    {
        // TODO: Remove? Clean? Be sane?
        level->endTurn(this);
        combatState = CombatState::Waiting;
    }
}

void Character::combat_DecideAction_MoveToLocation(int x, int y)
{
    // We have decided to move to a position, so we will set it up as our path
    schema.currentActionPoints--;
    pathToPosition(x, y);
    combatState = CombatState::PerformAction;
    combatAction = CombatAction::Move;
    combatTilesTraversed = 0;
}

void Character::combat_PerformAction_MoveToLocation(sf::Time &gameTime)
{
    // If I am moving for my combat action, follow the path as long as it exists
    if(path.size() > 0)
    {
        // If I have traversed the maximum number of tiles I can in this action
        if(combatTilesTraversed >= schema.speed)
        {
            // Move to the delay state to end the current action
            combatTilesTraversed = 0;
            combatState = CombatState::Delay;
        }

        // Else if its time to move
        else if(moveTimer.update(gameTime))
        {
            // Get the next tile from the movement path
            Tile* nextTile = path.back();
            // If that next path is not my current position
            if(nextTile->schema.x != Body::schema.x || nextTile->schema.y != Body::schema.y)
            {
                // If I have successfully moved to that position
                if(moveToTile(nextTile))
                {
                    // Ready the next position and increment by distance travelled
                    path.pop_back();
                    combatTilesTraversed++;
                }
            }
            // If I am on this tile already
            else
            {
                // Ready the next position but do not incremement distance travelled
                path.pop_back();
            }
        }
    }
    else
    {
        // If I have reached my destination head back to make my next decision
        combatState = CombatState::Delay;
    }
}

void Character::combat_DecideAction_AttackCharacter(Character* character)
{
    schema.currentActionPoints--;
    // TODO: do this
    targetEnemy = character;
    combatState = CombatState::PerformAction;
    combatAction = CombatAction::Attack;
}

void Character::combat_PerformAction_AttackCharacter(sf::Time &gameTime)
{
    // TODO: do this
    rangedAttack(targetEnemy);
    combatState = CombatState::Delay;
}

///////////////////////////////////////////////////////
//              CHARACTER EFFECTS                    //
///////////////////////////////////////////////////////

bool Character::isHostileTowards(Character* character)
{
    return hostilityCheckAi ? hostilityCheckAi(this, character) : false;
}

void Character::followPath(sf::Time &gameTime)
{
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
}

void Character::kill()
{
    schema.health = 0;

    // move equipment to inventory
    for(unsigned int i=0; i < equipment.size(); i++)
    {
        unequip(static_cast<Character::EquipmentSlot>(i));
    }
}

void Character::rangedAttack(Character* character)
{
    character->harm(100);
}

void Character::harm(int damage)
{
    schema.health = bit::Math::clamp(schema.health - damage, 0, schema.maxHealth);
    if(schema.health == 0)
        kill();
}

void Character::detectCombatEnter()
{
    isHostileCombatDetected = false;
    if(combatDetectionAi)
    {
        bool currentState = isHostileCombatDetected;
        isHostileCombatDetected = combatDetectionAi(this);

        // If I was not in combat but now I am
        if(!currentState && isHostileCombatDetected)
        {
            level->enterCombat();
        }
    }
}

void Character::detectCombatExit()
{
    isHostileCombatDetected = false;
    if(combatDetectionAi)
    {
        isHostileCombatDetected = combatDetectionAi(this);
    }
}

///////////////////////////////////////////////////////
//                 INSPECTION                        //
///////////////////////////////////////////////////////


void Character::inspectVisibleTiles(std::function<void(Tile* t)> inspector)
{
    Character* character = this;
    bit::Shadowcaster::computeFoV(Body::schema.x / level->tileWidth, Body::schema.y / level->tileHeight, level->tileColumns, level->tileRows, visionRadius,
        [character, inspector] (int x, int y, float distance) {
            Tile* tile = character->level->getTileAtIndices(x, y);
            if(tile && tile->metadata_shadowcastId != bit::Shadowcaster::shadowcastId)
            {
                tile->metadata_shadowcastId = bit::Shadowcaster::shadowcastId;
                inspector(tile);
            }
        },
        std::bind(&Character::inspectTileVisuallyBlocked, this, std::placeholders::_1, std::placeholders::_2)
    );
}

void Character::inspectVisibleCharacters(std::function<void(Character* character)> inspector, bool onlyLiving)
{
    Character* character = this;
    bit::Shadowcaster::computeFoV(Body::schema.x / level->tileWidth, Body::schema.y / level->tileHeight, level->tileColumns, level->tileRows, visionRadius,
        [character, inspector, onlyLiving] (int x, int y, float distance) {
            Tile* tile = character->level->getTileAtIndices(x, y);
            if(tile && tile->metadata_shadowcastId != bit::Shadowcaster::shadowcastId)
            {
                tile->metadata_shadowcastId = bit::Shadowcaster::shadowcastId;
                Body* body = tile->body;
                if(body && body->schema.type == Body::Type::Character)
                {
                    Character* occupant = static_cast<Character*>(body);
                    if(occupant != character)
                    {
                        if(!onlyLiving || (onlyLiving && !occupant->schema.isDead()))
                        {
                            inspector(occupant);
                        }
                    }
                }
            }
        },
        std::bind(&Character::inspectTileVisuallyBlocked, this, std::placeholders::_1, std::placeholders::_2)
    );
}

bool Character::inspectTileVisuallyBlocked(int x, int y)
{
    Tile* t = level->getTileAtPosition(x * level->tileWidth, y * level->tileHeight);
    return isTileVisuallyBlocked(t);
}

bool Character::isTileVisuallyBlocked(Tile* t)
{
    return (t && t->body && t->body->blockFoV);
}

///////////////////////////////////////////////////////
//                  INVENTORY                        //
///////////////////////////////////////////////////////

bool Character::equipFromInventory(EquipmentSlot slot, unsigned int itemId)
{
    Item* item = inventory->findItem(itemId);
    if(item)
    {
        switch(slot)
        {
            case EquipmentSlot::Head:
                if(!bit::Math::bitwiseHasAny(item->schema.CategoryArmor, ItemCategory::Armor::ArmorHead))
                    return false;
                break;
            case EquipmentSlot::Chest:
                if(!bit::Math::bitwiseHasAny(item->schema.CategoryArmor, ItemCategory::Armor::ArmorChest))
                    return false;
                break;
            case EquipmentSlot::Legs:
                if(!bit::Math::bitwiseHasAny(item->schema.CategoryArmor, ItemCategory::Armor::ArmorLeg))
                    return false;
                break;
            case EquipmentSlot::Feet:
                if(!bit::Math::bitwiseHasAny(item->schema.CategoryArmor, ItemCategory::Armor::ArmorFoot))
                    return false;
                break;
            case EquipmentSlot::Hands:
                if(!bit::Math::bitwiseHasAny(item->schema.CategoryArmor, ItemCategory::Armor::ArmorHand))
                    return false;
                break;
            case EquipmentSlot::Totem:
                if(!bit::Math::bitwiseHasAny(item->schema.CategoryJewelry, ItemCategory::Jewelry::JewelryTotem))
                    return false;
                break;
            case EquipmentSlot::WeaponPrimary:
                if(!bit::Math::bitwiseHasAny(item->schema.CategoryWeapon, ItemCategory::Weapon::WeaponRanged))
                    return false;
                break;
            case EquipmentSlot::WeaponSecondary:
                if(!bit::Math::bitwiseHasAny(item->schema.CategoryWeapon, ItemCategory::Weapon::WeaponMelee))
                    return false;
                break;
        }

        inventory->removeItem(itemId);
        unequip(slot);
        return equip(slot, item);
    }
    return false;
}

bool Character::equip(EquipmentSlot slot, Item* item)
{
    Item* equippedItem = equipment[slot];
    if(!equippedItem)
    {
        equipment[slot] = item;
        schema.equipmentIds[slot] = item->schema.id;
        return true;
    }
    return false;
}

void Character::unequip(EquipmentSlot slot)
{
    Item* equippedItem = equipment[slot];
    if(equippedItem)
    {
        inventory->addItem(equippedItem);
        schema.equipmentIds[slot] = 0;
        equipment[slot] = NULL;
    }
}

///////////////////////////////////////////////////////
//                   MOVEMENT                        //
///////////////////////////////////////////////////////

bool Character::moveUp()
{
    path.clear();
    return moveToPosition(Body::schema.x, Body::schema.y - level->tileHeight);
}

bool Character::moveDown()
{
    path.clear();
    return moveToPosition(Body::schema.x, Body::schema.y + level->tileHeight);
}

bool Character::moveLeft()
{
    path.clear();
    return moveToPosition(Body::schema.x - level->tileWidth, Body::schema.y);
}

bool Character::moveRight()
{
    path.clear();
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
    moveTimer.max();
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

///////////////////////////////////////////////////////
//                   MISSION                         //
///////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////
//                  MANAGEMENT                       //
///////////////////////////////////////////////////////

void Character::setControllingPlayer(Player* player)
{
    schema.isPlayerCharacter = true;
    schema.clientId = player->clientId;
    schema.player = player;
}

void Character::getAvailableInteractions(std::vector<Interaction::Type> &fill)
{
    if(schema.health > 0)
    {
        Body::getAvailableInteractions(fill);
    }
    else
    {
        fill.push_back(Interaction::Type::OpenInventory);
    }
}

void Character::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    // Body
    Body::prepareSnapshot(packet, full);

    // Character
    packet << schema;

    // Equipment
    for(unsigned int i=0; i < EquipmentSlot::_count; i++)
    {
        Item* equippedItem = equipment[i];
        if(equippedItem)
        {
            packet << true;
            equippedItem->prepareSnapshot(packet);
        }
        else
        {
            packet << false;
        }
    }

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