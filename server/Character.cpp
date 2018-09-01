#include "Character.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/System.hpp"
#include "ServerEvent.hpp"
#include "GameplayServer.hpp"
#include "Level.hpp"
#include "Body.hpp"
#include "Tile.hpp"
#include "Structure.hpp"
#include "Player.hpp"
#include "CharacterEffect.hpp"
#include "mission/Mission.hpp"
#include "items/Item.hpp"
#include "AccessLevel.hpp"
#include "RpgSystem.hpp"

Character::Character()
    : Body(), combatState(CombatState::Waiting), combatAction(CombatAction::MoveToLocation), actionDelayTimer(.75f), hostilityCheckAi(NULL), combatDetectionAi(NULL), combatDecisionAi(NULL),
      isHostileCombatDetected(false), hasTargetEnemy(false), targetEnemyPosition(0, 0), moveTimer(.5f), directionToMove(MoveDirection::NoDirection), equipment(), schema(), visionRadius(30), consumptionHeal(10)
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

    for (unsigned int i = 0; i < activeEffects.size(); i++)
    {
        delete activeEffects[i];
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
    Body::schema.obstructionRatio = .25;
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
    followPath(gameTime);
}

void Character::updateAlive_Combat(sf::Time &gameTime)
{
// EXPERIMENTAL!: I added this to improve turn queue processing time
// lots of enemies in update cycles takes several seconds since entire level has to be in turn queue
COMBAT_UPDATE:
    switch(combatState)
    {
        case CombatState::Waiting:

            // Its my turn
            if(this == level->turnQueue.front())
            {
                // Refill my action points and move to decision making
                schema.currentActionPoints = schema.maxActionPoints;
                schema.combatTilesTraversed = 0;
                sendCombatTurnStart();
                combat_SwitchStateDecide();

                // EXPERIMENTAL!
                if (!schema.isPlayerCharacter)
                    goto COMBAT_UPDATE;
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

                // EXPERIMENTAL!
                if (!schema.isPlayerCharacter)
                    goto COMBAT_UPDATE;
            }
            else
            {
                // If I am out of action points, then resume waiting my turn and signal my turn is over
                level->endTurn(this);
                combat_SwitchStateWaiting();

                // EXPERIMENTAL!
                if (!schema.isPlayerCharacter)
                    goto COMBAT_UPDATE;

                // Send game event to player for end of turn
                sendCombatTurnEnd();
            }
            break;

        case CombatState::PerformAction:

            switch(combatAction)
            {
                case CombatAction::MoveToLocation:

                    combat_PerformAction_MoveToLocation(gameTime);

                    // EXPERIMENTAL!
                    if (!schema.isPlayerCharacter && !isHostileCombatDetected)
                        goto COMBAT_UPDATE;

                    break;

                case CombatAction::MoveInDirection:

                    combat_PerformAction_MoveInDirection(gameTime);

                    // EXPERIMENTAL!
                    if (!schema.isPlayerCharacter && !isHostileCombatDetected)
                        goto COMBAT_UPDATE;

                    break;

                case CombatAction::Attack:

                    combat_PerformAction_AttackCharacter(gameTime);

                    // EXPERIMENTAL!
                    if (!schema.isPlayerCharacter && !isHostileCombatDetected)
                        goto COMBAT_UPDATE;

                    break;

                case CombatAction::Skip:

                    combat_PerformAction_Skip(gameTime);

                    // EXPERIMENTAL!
                    if (!schema.isPlayerCharacter && !isHostileCombatDetected)
                        goto COMBAT_UPDATE;

                    break;

                case CombatAction::SwapWeapon:

                    combat_PerformAction_SwapWeapon(gameTime);

                    // EXPERIMENTAL!
                    if (!schema.isPlayerCharacter && !isHostileCombatDetected)
                        goto COMBAT_UPDATE;

                    break;

                case CombatAction::UsedItem:

                    combat_PerformAction_UsedItem(gameTime);

                    // EXPERIMENTAL!
                    if (!schema.isPlayerCharacter && !isHostileCombatDetected)
                        goto COMBAT_UPDATE;

                    break;
            }

            break;

        case CombatState::Delay:

            // Allows us to wait before next action point usage for delay between actions
            // Helps the simulation feel natural
            // We only want this delay to fire for characters who are within hostile attack range of a player
            if (!isHostileCombatDetected) {
                combat_SwitchStateDecide();
                actionDelayTimer.reset();

                // EXPERIMENTAL!
                if (!schema.isPlayerCharacter)
                    goto COMBAT_UPDATE;
            }
            else if(actionDelayTimer.update(gameTime))
            {
                combat_SwitchStateDecide();
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
        combat_DecideAction_Skip();
    }
}

void Character::combat_DecideAction_MoveInDirection(MoveDirection moveDirection)
{
    // See if i can move in this direction
    bool can = false;
    switch (moveDirection)
    {
        case MoveDirection::Up:
            can = canPathToPosition(Body::schema.x, Body::schema.y - level->tileHeight);
            break;
        case MoveDirection::Down:
            can = canPathToPosition(Body::schema.x, Body::schema.y + level->tileHeight);
            break;
        case MoveDirection::Left:
            can = canPathToPosition(Body::schema.x - level->tileWidth, Body::schema.y);
            break;
        case MoveDirection::Right:
            can = canPathToPosition(Body::schema.x + level->tileWidth, Body::schema.y);
            break;
    }
    if (can)
    {
        directionToMove = moveDirection;
        combatAction = CombatAction::MoveInDirection;
        combat_SwitchStatePerform();
    }
}

void Character::combat_PerformAction_MoveInDirection(sf::Time &gameTime)
{
    switch (directionToMove)
    {
        case MoveDirection::Up:
            moveUp();
            break;
        case MoveDirection::Down:
            moveDown();
            break;
        case MoveDirection::Left:
            moveLeft();
            break;
        case MoveDirection::Right:
            moveRight();
            break;
    }
    directionToMove = MoveDirection::NoDirection;
    schema.combatTilesTraversed++;
    if (schema.combatTilesTraversed >= schema.speed)
    {
        // Move to the delay state to end the current action
        schema.currentActionPoints--;
        schema.combatTilesTraversed = 0;
    }
    combat_SwitchStateDelay();
}

void Character::combat_DecideAction_MoveToLocation(int x, int y)
{
    // We have decided to move to a position, so we will set it up as our path
    schema.currentActionPoints--;
    pathToPosition(x, y);
    combatAction = CombatAction::MoveToLocation;
    combat_SwitchStatePerform();
}

void Character::combat_PerformAction_MoveToLocation(sf::Time &gameTime)
{
    // If I am moving for my combat action, follow the path as long as it exists
    if(path.size() > 0)
    {
        // If I have traversed the maximum number of tiles I can in this action
        if(schema.combatTilesTraversed >= schema.speed)
        {
            // Move to the delay state to end the current action
            schema.combatTilesTraversed = 0;
            combat_SwitchStateDelay();

            // Clear the path since I cannot travel further
            path.clear();
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

                    // Our path may have been cleared by tile event (like changing levels)
                    if(path.size() > 0)
                    {
                        path.pop_back();
                    }
                    schema.combatTilesTraversed++;
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
        schema.combatTilesTraversed = 0;
        combat_SwitchStateDelay();
    }
}

void Character::combat_DecideAction_AttackCharacter(Character* character)
{
    hasTargetEnemy = true;
    targetEnemyPosition.x = character->Body::schema.x;
    targetEnemyPosition.y = character->Body::schema.y;

    schema.currentActionPoints--;
    schema.combatTilesTraversed = 0;
    combatAction = CombatAction::Attack;
    combat_SwitchStatePerform();
}

void Character::combat_PerformAction_AttackCharacter(sf::Time &gameTime)
{
    if(hasTargetEnemy)
    {
        Character* targetEnemy = level->getCharacterByPosition(targetEnemyPosition.x, targetEnemyPosition.y);

        if(targetEnemy)
        {
            attack(targetEnemy);

            // Set the direction
            sf::Vector2f dir = bit::VectorMath::directionToVector(Body::schema.x, Body::schema.y, targetEnemy->Body::schema.x, targetEnemy->Body::schema.y);
            schema.direction.x = dir.x;
            schema.direction.y = dir.y;
        }
    }

    // End performance
    combat_SwitchStateDelay();
}

void Character::combat_DecideAction_Skip()
{
    schema.currentActionPoints--;
    schema.combatTilesTraversed = 0;
    combatAction = CombatAction::Skip;
    combat_SwitchStatePerform();
}

void Character::combat_PerformAction_Skip(sf::Time &gameTime)
{
    combat_SwitchStateDelay();
}

void Character::combat_DecideAction_SwapWeapon()
{
    // costless
    combatAction = CombatAction::SwapWeapon;
    combat_SwitchStatePerform();
}

void Character::combat_PerformAction_SwapWeapon(sf::Time &gameTime)
{
    swapWeapons();
    combat_SwitchStateDelay();
}

void Character::combat_DecideAction_UsedItem()
{
    // used an item external to the character
    schema.currentActionPoints--;
    schema.combatTilesTraversed = 0;
    combatAction = CombatAction::UsedItem;
    combat_SwitchStatePerform();
}

void Character::combat_PerformAction_UsedItem(sf::Time &gameTime)
{
    // used an item external to the character
    combat_SwitchStateDelay();
}



void Character::combat_SwitchStateWaiting()
{
    combatState = CombatState::Waiting;
}

void Character::combat_SwitchStateDecide()
{
    if(combatState != CombatState::Delay || schema.currentActionPoints > 0)
    {
        sendCombatDecisionStart();
    }

    combatState = CombatState::DecideAction;
}

void Character::combat_SwitchStatePerform()
{
    sendCombatDecisionEnd();

    combatState = CombatState::PerformAction;
}

void Character::combat_SwitchStateDelay()
{
    combatState = CombatState::Delay;
}

void Character::distributedUpdate(sf::Time &gameTime)
{
    Body::distributedUpdate(gameTime);

    if(!schema.isDead())
    {
        distributedUpdateAlive(gameTime);
    }
    else
    {
        distributedUpdateDead(gameTime);
    }
}

void Character::distributedUpdateAlive(sf::Time &gameTime)
{
    switch(level->state)
    {
        case Level::State::Free:
            distributedUpdateAlive_Free(gameTime);
            break;

        case Level::State::Combat:
            distributedUpdateAlive_Combat(gameTime);
            break;
    }
}

void Character::distributedUpdateAlive_Free(sf::Time &gameTime)
{
    // Check for combatable scenario to begin
    detectCombatEnter();
}

void Character::distributedUpdateAlive_Combat(sf::Time &gameTime)
{
}

void Character::distributedUpdateDead(sf::Time &gameTime)
{
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
                // Our path may have been cleared by tile event (like changing levels)
                if(path.size() > 0)
                {
                    path.pop_back();
                }
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
    // Change stats
    schema.health = 0;

    // Trigger event
    onDeath.trigger<Character*>(this);

    // Move equipment to inventory
    for(unsigned int i=0; i < equipment.size(); i++)
    {
        unequip(static_cast<Character::EquipmentSlot>(i));
    }

    // Send event
    level->sendEventToAllPlayers([this] (bit::ServerPacket &packet) -> void {
        packet << sf::Uint32(ServerEvent::CharacterDeath);
        packet << this->Body::schema.x;
        packet << this->Body::schema.y;
        packet << sf::Uint32(this->schema.type);
    });
}

void Character::attack(Character* character)
{
    float CoH = RpgSystem::Combat::calculateChanceOfHit(this, character);

    // Visualize attack
    Item* weapon = equipment[Character::EquipmentSlot::WeaponPrimary];
    bool unarmedAttack = (weapon == NULL);

    if(weapon && weapon->onUse)
    {
        // Weapon use
        weapon->onUse(this);
    }

    if(bit::Math::randomFloat() < CoH)
    {
        float damage = RpgSystem::Combat::calculateAttackDamage(this, character);
        unsigned int mitigatedDamage = RpgSystem::Combat::calculateDamageMitigation(this, character, damage);
        character->harm(mitigatedDamage);

        // Apply any special attack to enemy
        if (weapon && weapon->onAttack)
        {
            // Weapon attack
            weapon->onAttack(this, character);
        }

        // Game Event
        if(schema.isPlayerCharacter)
        {
            Character* me = this;
            level->server->sendEventToClient(*schema.player->client, [me, character, mitigatedDamage](bit::ServerPacket &packet){
                packet << sf::Uint32(ServerEvent::PlayerAttacksCharacter);
                packet << sf::Uint32(character->schema.type);
                packet << sf::Uint32(mitigatedDamage);
            });
        }
        else if(character->schema.isPlayerCharacter)
        {
            Character* me = this;
            level->server->sendEventToClient(*character->schema.player->client, [me, character, mitigatedDamage](bit::ServerPacket &packet){
                packet << sf::Uint32(ServerEvent::CharacterAttacksPlayer);
                packet << sf::Uint32(me->schema.type);
                packet << sf::Uint32(mitigatedDamage);
            });
        }

        // Game event
        // Unarmed attack hit
        if (unarmedAttack && schema.isPlayerCharacter)
        {
            level->sendEventToAllPlayers([character](bit::ServerPacket &packet) {
                packet << sf::Uint32(ServerEvent::BluntHit);
                packet << character->Body::schema.x;
                packet << character->Body::schema.y;
            });
        }
    }
    else
    {
        // Game Event
        if(schema.isPlayerCharacter)
        {
            Character* me = this;
            level->server->sendEventToClient(*schema.player->client, [me, character](bit::ServerPacket &packet){
                packet << sf::Uint32(ServerEvent::PlayerMissesCharacter);
                packet << sf::Uint32(character->schema.type);
            });
        }
        else if(character->schema.isPlayerCharacter)
        {
            Character* me = this;
            level->server->sendEventToClient(*character->schema.player->client, [me, character](bit::ServerPacket &packet){
                packet << sf::Uint32(ServerEvent::CharacterMissesPlayer);
                packet << sf::Uint32(me->schema.type);
            });
        }

        // Unarmed Miss Event
        if (unarmedAttack)
        {
            level->sendEventToAllPlayers([character](bit::ServerPacket &packet) {
                packet << sf::Uint32(ServerEvent::BluntMiss);
                packet << character->Body::schema.x;
                packet << character->Body::schema.y;
            });
        }
    }
}

void Character::heal(int amount)
{
    schema.health = bit::Math::clamp(schema.health + amount, 0, schema.maxHealth);

    // Event
    level->sendEventToAllPlayers([this](bit::ServerPacket &packet) {
        packet << sf::Uint32(ServerEvent::CharacterHeal);
        packet << this->Body::schema.x;
        packet << this->Body::schema.y;
    });
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


bool Character::canDodge()
{
    return true;
}

bool Character::canBlock()
{
    return (equipment[EquipmentSlot::WeaponPrimary] && equipment[EquipmentSlot::WeaponPrimary]->schema.isOfWeaponType(ItemCategory::Weapon::WeaponMelee));
}

void Character::addEffect(CharacterEffect* effect)
{
    // Only allowed one effect of each kind per character
    for (unsigned int i = 0; i < activeEffects.size(); i++)
    {
        if (activeEffects[i]->type == effect->type)
        {
            delete effect;
            return;
        }
    }

    // If its the only one of its kind, add it
    effect->onStart(this);
    activeEffects.push_back(effect);
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

void Character::inspectVisibleStructures(std::function<void(::Structure* s)> inspector, bool onlyNonBlocking)
{
    Character* character = this;
    bit::Shadowcaster::computeFoV(Body::schema.x / level->tileWidth, Body::schema.y / level->tileHeight, level->tileColumns, level->tileRows, visionRadius,
        [character, inspector, onlyNonBlocking] (int x, int y, float distance) {
            Tile* tile = character->level->getTileAtIndices(x, y);
            if(tile && tile->metadata_shadowcastId != bit::Shadowcaster::shadowcastId)
            {
                tile->metadata_shadowcastId = bit::Shadowcaster::shadowcastId;

                Body* body = tile->body;
                if(body && body->schema.type == Body::Type::Structure)
                {
                    ::Structure* structure = static_cast<::Structure*>(body);

                    if(!onlyNonBlocking || (onlyNonBlocking && !structure->Body::blockFoV))
                    {
                        inspector(structure);
                    }
                }
            }
        },
        std::bind(&Character::inspectTileVisuallyBlocked, this, std::placeholders::_1, std::placeholders::_2)
    );
}

void Character::inspectCombatReachableTiles(std::function<void(Tile* t)> inspector)
{
    Character* character = this;
    int originX = character->Body::schema.x / character->level->tileWidth;
    int originY = character->Body::schema.y / character->level->tileHeight;
    int maxDistance = character->schema.speed; // TODO - shouldn't this subtract my current combatTilesTraversed variable?s

    // Flood fill
    bit::FloodFill::compute(originX, originY,
        [character, maxDistance, inspector] (int x, int y, int depth) { // inspection
            Tile* t = character->level->getTileAtIndices(x, y);
            if(t && t->metadata_floodfillId != bit::FloodFill::floodfillId)
            {
                t->metadata_floodfillId = bit::FloodFill::floodfillId;
                inspector(t);
            }
        },
        [character, maxDistance] (int x, int y, int depth) { // isBlocked
            return depth > maxDistance || character->inspectTileMobilityBlocked(x, y);
        }
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

bool Character::inspectTileMobilityBlocked(int x, int y)
{
    Tile* t = level->getTileAtPosition(x * level->tileWidth, y * level->tileHeight);
    return isTileMobilityBlocked(t);
}

bool Character::isTileMobilityBlocked(Tile* t)
{
    return isTileBlockedForPathfinding(t);
}

void Character::inspectLineOfSightTiles(int endX, int endY, std::function<void(Tile* t)> inspector)
{
    Character* character = this;
    level->raycastTiles(Body::schema.x, Body::schema.y, endX, endY, [character, &inspector] (Tile* t) -> bool {
        // Inspect all tiles that at minimum do not contain myself
        if(t->body != character)
            inspector(t);
        // Stop the raycase if we hit a body that stops vision such as a wall
        return (t->body && t->body->blockFoV);
    });
}

void Character::inspectLineOfSightBodies(int endX, int endY, std::function<void(Body* b)> inspector)
{
    Character* character = this;
    inspectLineOfSightTiles(endX, endY, [character, &inspector] (Tile* t) {
        if(t->body)
            inspector(t->body);
    });
}

void Character::inspectLineOfSightCharacters(int endX, int endY, std::function<void(Character* c)> inspector, bool onlyLiving)
{
    Character* character = this;
    inspectLineOfSightBodies(endX, endY, [character, &inspector] (Body* b) {
        if(b->schema.type == Body::Character)
        {
            Character* occupant = static_cast<Character*>(b);
            inspector(occupant);
        }
    });
}

Character* Character::getClosestVisibleEnemy()
{
    Character* self =  this;
    Character* closestEnemy = NULL;
    float closestDistance = 0;
    inspectVisibleCharacters([self, &closestEnemy, &closestDistance] (Character* occupant) {
        int x1 = self->Body::schema.x;
        int y1 = self->Body::schema.y;
        int width1 = self->Body::schema.width;
        int height1 = self->Body::schema.height;

        int x2 = occupant->Body::schema.x;
        int y2 = occupant->Body::schema.y;
        int width2 = occupant->Body::schema.width;
        int height2 = occupant->Body::schema.height;

        // If the occupant is not adjacent to me then move towards them
        float distance = bit::RectangleMath::distance(x1, y1, width1, height1, x2, y2, width2, height2);
        if(self->isHostileTowards(occupant) && (closestEnemy == NULL || distance < closestDistance))
        {
            closestEnemy = occupant;
            closestDistance = distance;
        }
    });

    return closestEnemy;
}


///////////////////////////////////////////////////////
//                  INVENTORY                        //
///////////////////////////////////////////////////////

bool Character::slotAcceptsItem(EquipmentSlot slot, Item* item)
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
            if(!bit::Math::bitwiseHasAny(item->schema.CategoryWeapon, ItemCategory::Weapon::WeaponRanged | ItemCategory::Weapon::WeaponMelee))
                return false;
            break;
        case EquipmentSlot::WeaponSecondary:
            if(!bit::Math::bitwiseHasAny(item->schema.CategoryWeapon, ItemCategory::Weapon::WeaponRanged | ItemCategory::Weapon::WeaponMelee))
                return false;
            break;
    }
    return true;
}

// Utility method for undoing a slot
void Character::voidEquipmentSlot(EquipmentSlot slot)
{
    schema.equipmentIds[slot] = 0;
    equipment[slot] = NULL;
}

// Utility method for setting a slot
void Character::setEquipmentSlot(EquipmentSlot slot, Item* item)
{
    equipment[slot] = item;
    schema.equipmentIds[slot] = item->schema.id;
}

// Takes a destination slot and an inventory item id
// If the item exists, validates its of an acceptable type
// If there is a slot item it removes it and puts it in the same position as incoming item
// Moves the incoming item to the slot
bool Character::equipFromInventory(EquipmentSlot slot, unsigned int itemId)
{
    // If the item exists and is acceptable for the slot
    Item* item = inventory->findItem(itemId);
    if(item && slotAcceptsItem(slot, item))
    {
        // Remove item from inventory
        unsigned int position = item->schema.position;
        removeItemFromInventory(itemId);

        // If we have equipment swap it with the item
        if(equipment[slot])
        {
            Item* equipped = equipment[slot];
            unequipToPosition(slot, position);
        }

        // Add the item to the slot
        return equip(slot, item);
    }
    return false;
}

// Takes a destination slot and an loot item id
// If the item exists, validates its of an acceptable type
// If there is a slot item it removes it and puts it in the same position as incoming item
// Moves the incoming item to the slot
bool Character::equipFromLoot(EquipmentSlot slot, unsigned int itemId)
{
    if(inventoryHost)
    {
        // If the item exists and is acceptable for the slot
        Item* theirItem = inventoryHost->findItemInInventory(itemId);
        if(theirItem && slotAcceptsItem(slot, theirItem))
        {
            // Remove item from their inventory
            unsigned int position = theirItem->schema.position;
            inventoryHost->removeItemFromInventory(theirItem->schema.id);

            // If we have equipment swap it with the item
            if(equipment[slot])
            {
                // Remove my equipment
                Item* equipped = equipment[slot];
                voidEquipmentSlot(slot);
                sendEquipmentRemovedEvent(slot);

                // Add it to the host's inventory at their position
                inventoryHost->addItemToInventoryAtPosition(equipped, position);
            }

            // Add their item to the slot
            return equip(slot, theirItem);
        }
        return false;
    }
}

// Takes any item and sets it to the specified slot
// This does not validate anything
// Sends the equip network event
bool Character::equip(EquipmentSlot slot, Item* item)
{
    // Set the slot up
    setEquipmentSlot(slot, item);
    sendEquipmentAddedEvent(slot); // netevent
    return true;
}

// Takes a slot and moves its item to the inventory position available
// Does not validate anything
// Sends a network event for removal
void Character::unequip(EquipmentSlot slot)
{
    if(equipment[slot])
    {
        addItemToInventory(equipment[slot]);
        voidEquipmentSlot(slot);
        sendEquipmentRemovedEvent(slot); // netevent
    }
}

// Takes a slot and moves its item to the inventory at the position specified
// Does not validate anything
// Sends a network event for removal
void Character::unequipToPosition(EquipmentSlot slot, unsigned int position)
{
    if(equipment[slot])
    {
        addItemToInventoryAtPosition(equipment[slot], position);
        voidEquipmentSlot(slot);
        sendEquipmentRemovedEvent(slot); // netevent
    }
}

// Will take both weapon slots and swap them
// This is used for in combat mainly
void Character::swapWeapons()
{
    Item* primary = equipment[Character::EquipmentSlot::WeaponPrimary];
    Item* secondary = equipment[Character::EquipmentSlot::WeaponSecondary];
    
    if(primary)
        setEquipmentSlot(Character::EquipmentSlot::WeaponSecondary, primary);
    if(secondary)
        setEquipmentSlot(Character::EquipmentSlot::WeaponPrimary, secondary);
}

// Takes an existing inventory item and moves its inventory position
// If another items exists at the new location they are swapped
bool Character::moveItemToPosition(unsigned int itemId, unsigned int position)
{
    // Find one by id and the other by position
    Item* existing = findItemByPosition(position);
    Item* item = findItemInInventory(itemId);

    // If I had an item at the position
    if(existing && item)
    {
        // Switch positions on the items
        existing->schema.position = item->schema.position;
        item->schema.position = position;
        sendItemUpdateEvent(existing); // netevent
        sendItemUpdateEvent(item); // netevent
    }
    else if(item)
    {
        // Just set the new position
        item->schema.position = position;
        sendItemUpdateEvent(item); // netevent
    }

    return true;
}

// Takes an existing loot item and moves its loot position
// If another items exists at the new location they are swapped
// Basically rearranges the accessing container
bool Character::moveLootItemToLootPosition(unsigned int itemId, unsigned int position)
{
    if(inventoryHost)
    {
        // Find one by id and the other by position
        Item* existing = inventoryHost->findItemByPosition(position);
        Item* item = inventoryHost->findItemInInventory(itemId);

        // If I had an item at the position
        if(existing && item)
        {
            // Switch positions on the items
            existing->schema.position = item->schema.position;
            item->schema.position = position;
        }
        else if(item)
        {
            // Just set the new position
            item->schema.position = position;
        }
    }

    return true;
}

// Takes equipment and moves it to the inventory at the specified position
// If an item is present there it will need to check that it is acceptable
// If so it will swap them
// If there is no item there it will set the item at the specified position
bool Character::moveEquipmentToPosition(EquipmentSlot slot, unsigned int position)
{
    // Find our items in question
    Item* equippedItem = equipment[slot];
    Item* itemAtPosition = findItemByPosition(position);

    if(equippedItem)
    {
        // If we have an item at the position already
        if(itemAtPosition)
        {
            // Check that it is an acceptable item to swap
            if(slotAcceptsItem(slot, itemAtPosition))
            {
                // Remove equipment
                voidEquipmentSlot(slot);
                sendEquipmentRemovedEvent(slot); // netevent

                // Add item to equipment slot
                removeItemFromInventory(itemAtPosition->schema.id);
                equip(slot, itemAtPosition); // netevent

                // Add equipment to inventory at the slot specified
                addItemToInventoryAtPosition(equippedItem, position);

                return true;
            }
            else
            {
                return false;
            }
        }
        else
        {
            // Remove equipment
            voidEquipmentSlot(slot);
            sendEquipmentRemovedEvent(slot); // netevent

            // Add equipment to inventory at the slot specified
            addItemToInventoryAtPosition(equippedItem, position);
            
            return true;
        }
    }

    return false;
}

// Takes two slots, checks their compatibility and swaps their locations
bool Character::swapEquipment(EquipmentSlot slotA, EquipmentSlot slotB)
{
    // See if we have items in both slots
    Item* itemA = equipment[slotA];
    Item* itemB = equipment[slotB];

    // If we have both items
    if(itemA && itemB)
    {
        // Check that both are compatible
        if(slotAcceptsItem(slotA, itemB) && slotAcceptsItem(slotB, itemA))
        {
            // Swap them directly
            setEquipmentSlot(slotA, itemB);
            setEquipmentSlot(slotB, itemA);
            sendEquipmentSwappedEvent(slotA, slotB); // netevent
            return true;
        }
    }
    // If we only have item A
    else if(itemA)
    {
        // Check that slot B accepts Item A
        if(slotAcceptsItem(slotB, itemA))
        {
            // Remove item from slot A
            voidEquipmentSlot(slotA);
            sendEquipmentRemovedEvent(slotA);

            // Set it to slot B
            setEquipmentSlot(slotB, itemA);
            sendEquipmentAddedEvent(slotB);
        }
    }
    // If we only have item B
    else if(itemB)
    {
        // Check that slot A accepts Item B
        if(slotAcceptsItem(slotA, itemB))
        {
            // Remove item from slot B
            voidEquipmentSlot(slotB);
            sendEquipmentRemovedEvent(slotB);

            // Set it to slot A
            setEquipmentSlot(slotA, itemB);
            sendEquipmentAddedEvent(slotA);
        }
    }

    return false;
}

bool Character::moveLootItemToInventoryPosition(unsigned int itemId, unsigned int position)
{
    // If I have an inventory host
    if(inventoryHost)
    {
        // SWAP
        // See if I have an item at the postion
        Item* myItem = findItemByPosition(position);
        if(myItem)
        {
            Item* theirItem = inventoryHost->findItemInInventory(itemId);
            if(theirItem)
            {
                unsigned int theirPosition = theirItem->schema.position;

                // Take their item out
                inventoryHost->removeItemFromInventory(theirItem->schema.id);

                // Take my item from my inventory and add it to theirs
                removeItemFromInventory(myItem->schema.id);
                inventoryHost->addItemToInventoryAtPosition(myItem, theirPosition);

                // Add their item to my position
                addItemToInventoryAtPosition(theirItem, position);
                return true;
            }
        }
        // NO SWAP
        // If I dont have an item there
        else
        {
            Item* theirItem = inventoryHost->removeItemFromInventory(itemId);
            if(theirItem)
            {
                // Add their item to my inventory
                addItemToInventoryAtPosition(theirItem, position);
                return true;
            }
        }
    }

    return false;
}

bool Character::moveInventoryItemToLootPosition(unsigned int itemId, unsigned int position)
{
    // If I have an inventory host
    if(inventoryHost)
    {
        // SWAP
        // See if they have an item
        Item* theirItem = inventoryHost->findItemByPosition(position);
        if(theirItem)
        {
            // Make sure I have the item
            Item* myItem = findItemInInventory(itemId);
            if(myItem)
            {
                unsigned int myPosition = myItem->schema.position;

                // Take my item out
                removeItemFromInventory(myItem->schema.id);

                // Take their item from their inventory and add it to mine
                inventoryHost->removeItemFromInventory(theirItem->schema.id);
                addItemToInventoryAtPosition(theirItem, myPosition);

                // Add my item to their position
                inventoryHost->addItemToInventoryAtPosition(myItem, position);
                return true;
            }
        }
        // NO SWAP
        // If they have no item there
        else
        {
            Item* myItem = removeItemFromInventory(itemId);
            if(myItem)
            {
                // Add my item to their inventory
                inventoryHost->addItemToInventoryAtPosition(myItem, position);
                return true;
            }
        }
    }

    return false;
}


bool Character::moveEquipmentToLootPosition(EquipmentSlot slot, unsigned int position)
{
    // If I have an inventory host
    if(inventoryHost)
    {
        // SWAP
        // See if they have an item
        Item* theirItem = inventoryHost->findItemByPosition(position);
        if(theirItem)
        {
            // Check that their item will go in this slot
            if(!slotAcceptsItem(slot, theirItem))
            {
                return false;
            }

            // Make sure I have the equipment
            Item* myEquipment = equipment[slot];
            if(myEquipment)
            {
                // Take my equipment off
                voidEquipmentSlot(slot);
                sendEquipmentRemovedEvent(slot);

                // Take their item from their inventory and add it to my equipment
                inventoryHost->removeItemFromInventory(theirItem->schema.id);
                setEquipmentSlot(slot, theirItem);
                sendEquipmentAddedEvent(slot);

                // Add my old equipment to their position
                inventoryHost->addItemToInventoryAtPosition(myEquipment, position);
                return true;
            }
        }
        // NO SWAP
        // If they have no item there
        else
        {
            Item* myEquipment = equipment[slot];
            if(myEquipment)
            {
                // Take my equipment off
                voidEquipmentSlot(slot);
                sendEquipmentRemovedEvent(slot);

                // Add my old equipment to their inventory
                inventoryHost->addItemToInventoryAtPosition(myEquipment, position);
                return true;
            }
        }
    }

    return false;
}


void Character::sendInventoryUpdate()
{
    if(schema.isPlayerCharacter)
    {
        Character* c = this;
        level->server->sendEventToClient(*schema.player->client, [c](bit::ServerPacket &packet) {
            packet << sf::Uint32(ServerEvent::InventoryUpdate);
            c->inventory->prepareSnapshot(packet);
        });
    }
}

void Character::sendItemUpdateEvent(Item* item)
{
    if(schema.isPlayerCharacter)
    {
        level->server->sendEventToClient(*schema.player->client, [item](bit::ServerPacket &packet) {
            packet << sf::Uint32(ServerEvent::ItemUpdated);
            item->prepareSnapshot(packet);
        });
    }
}

void Character::sendEquipmentAddedEvent(EquipmentSlot slot)
{
    if(schema.isPlayerCharacter)
    {
        Item* item = equipment[slot];
        level->server->sendEventToClient(*schema.player->client, [item, slot](bit::ServerPacket &packet) {
            packet << sf::Uint32(ServerEvent::EquipmentAdded);
            packet << sf::Uint32(slot);
            item->prepareSnapshot(packet);
        });
    }
}

void Character::sendEquipmentUpdatedEvent(EquipmentSlot slot)
{
    if(schema.isPlayerCharacter)
    {
        Item* item = equipment[slot];
        level->server->sendEventToClient(*schema.player->client, [item, slot](bit::ServerPacket &packet) {
            packet << sf::Uint32(ServerEvent::EquipmentUpdated);
            packet << sf::Uint32(slot);
            item->prepareSnapshot(packet);
        });
    }
}

void Character::sendEquipmentRemovedEvent(EquipmentSlot slot)
{
    if(schema.isPlayerCharacter)
    {
        level->server->sendEventToClient(*schema.player->client, [slot](bit::ServerPacket &packet) {
            packet << sf::Uint32(ServerEvent::EquipmentRemoved);
            packet << sf::Uint32(slot);
        });
    }
}

void Character::sendEquipmentSwappedEvent(EquipmentSlot slotA, EquipmentSlot slotB)
{
    if(schema.isPlayerCharacter)
    {
        Item* itemA = equipment[slotA];
        Item* itemB = equipment[slotB];
        level->server->sendEventToClient(*schema.player->client, [itemA, slotA, itemB, slotB](bit::ServerPacket &packet) {
            packet << sf::Uint32(ServerEvent::EquipmentSwapped);
            packet << sf::Uint32(slotA);
            itemA->prepareSnapshot(packet);
            packet << sf::Uint32(slotB);
            itemB->prepareSnapshot(packet);
        });
    }
}

void Character::onInventoryOpen(Body* guest)
{
    if(schema.isDead())
    {
        if(guest && guest->schema.type == Body::Type::Character)
        {
            Character* guestCharacter = static_cast<Character*>(guest);
            if(guestCharacter->schema.isPlayerCharacter && guestCharacter->schema.player)
            {
                Player* player = guestCharacter->schema.player;
                level->sendEventToPlayer(player, [] (bit::ServerPacket &packet) -> void {
                    packet << sf::Uint32(ServerEvent::BodyOpen);
                });
            }
        }
    }
}

void Character::onInventoryClose(Body* guest)
{
    if(schema.isDead())
    {
        if(guest && guest->schema.type == Body::Type::Character)
        {
            Character* guestCharacter = static_cast<Character*>(guest);
            if(guestCharacter->schema.isPlayerCharacter && guestCharacter->schema.player)
            {
                Player* player = guestCharacter->schema.player;
                level->sendEventToPlayer(player, [] (bit::ServerPacket &packet) -> void {
                    packet << sf::Uint32(ServerEvent::BodyClose);
                });
            }
        }
    }
}

bool Character::hasAccess(AccessLevel accessLevel)
{
	// A character has access to an access level by KeyCards found
	Item* keycard = inventory->findItemByTypeAndAccessLevel(Item::KeyCard, accessLevel);
	return keycard != NULL;
}

unsigned int Character::getEffectiveRange()
{
    // Sees if I have a weapon equipped and returns its range
    // If no weapon, then I am unarmed and my range is 1
    Item* weapon = equipment[Character::EquipmentSlot::WeaponPrimary];
    return weapon ? weapon->schema.effectiveRangeInTiles : 1;
}

unsigned int Character::getMaximumRange()
{
    // Sees if I have a weapon equipped and returns its maximum range
    // If no weapon, then I am unarmed and my range is 1
    Item* weapon = equipment[Character::EquipmentSlot::WeaponPrimary];
    return weapon ? weapon->schema.maximumRangeInTiles : 1;
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

// Checks a target level x and y to see if I could occupy it
bool Character::canMoveToPosition(Level* targetLevel, float x, float y)
{
    std::vector<Tile*> newTiles;
    targetLevel->getTilesWithinRectangle(x, y, Body::schema.width, Body::schema.height, newTiles);

    // Check if I can move
    for(unsigned int i=0; i < newTiles.size(); i++)
    {
        if(isTileBlocked(newTiles[i]))
        {
            return false;
        }
    }

    return true;
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

        sf::Vector2f dir = bit::VectorMath::normalize(x - Body::schema.x, y - Body::schema.y);
        schema.direction.x = dir.x;
        schema.direction.y = dir.y;

        Body::schema.x = x;
        Body::schema.y = y;

        onMove();
    }

    return canMove;
}

void Character::onMove()
{
    // Events to run when the character moves

    // Character effects run per number of tiles
    // Update with allowance for adding / removing entities mid loop
    for (auto it = activeEffects.begin(); it != activeEffects.end();)
    {
        (*it)->run(this);

        // See if we need to remove the effect
        if ((*it)->isComplete())
        {
            (*it)->onEnd(this);
            delete (*it);
            it = activeEffects.erase(it);
        }
        else
        {
            ++it; // iterate list pointer
        }
    }
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

bool Character::canPathToPosition(float x, float y)
{
    std::vector<Tile*> v;
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
        v);
    return v.size() > 0;
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

void Character::clearLevelSpecificData()
{
    path.clear();
}

void Character::sendCombatTurnStart()
{
    // Send game event to player for start of turn
    if(schema.isPlayerCharacter)
    {
        level->server->sendEventToClient(*schema.player->client, [](bit::ServerPacket &packet){
            packet << sf::Uint32(ServerEvent::CombatTurnStart);
        });
    }
}

void Character::sendCombatTurnEnd()
{
    // Send game event to player for end of turn
    if(schema.isPlayerCharacter)
    {
        level->server->sendEventToClient(*schema.player->client, [](bit::ServerPacket &packet){
            packet << sf::Uint32(ServerEvent::CombatTurnEnd);
        });
    }
}

void Character::sendCombatDecisionStart()
{
    // Send game event to player for start of decision making

    if(schema.isPlayerCharacter)
    {
        Character* character = this;
        level->server->sendEventToClient(*schema.player->client, [character] (bit::ServerPacket &packet){
            packet << sf::Uint32(ServerEvent::CombatDecisionStart);
            
            // Calculate chance of hit for local enemies
            std::vector<Character*> characters;
            character->inspectVisibleCharacters([&characters](Character* c){
                characters.push_back(c);
            });
            packet << sf::Uint32(characters.size());
            for(unsigned int i=0; i < characters.size(); i++)
            {
                // Send basic tile information for the move markers
                packet << sf::Uint32(characters[i]->Body::schema.id);
                packet << RpgSystem::Combat::calculateChanceOfHit(character, characters[i]);
            }
        });
    }
}

void Character::sendCombatDecisionEnd()
{
    // Send game event to player for end of decision making
    if(schema.isPlayerCharacter)
    {
        level->server->sendEventToClient(*schema.player->client, [](bit::ServerPacket &packet){
            packet << sf::Uint32(ServerEvent::CombatDecisionEnd);
        });
    }
}

void Character::setControllingPlayer(Player* player)
{
    schema.isPlayerCharacter = true;
    schema.clientId = player->clientId;
    schema.player = player;
}

void Character::unsetControllingPlayer()
{
    schema.isPlayerCharacter = false;
    schema.clientId = 0;
    schema.player = NULL;
}

void Character::setSpectatingPlayer(Player* player)
{
    spectators.push_back(player);
}

void Character::unsetSpectatingPlayer(Player* player)
{
    spectators.erase(std::remove(spectators.begin(), spectators.end(), player), spectators.end());
}

void Character::clearSpectators()
{
    for(unsigned int i=0; i < spectators.size(); i++)
    {
        spectators[i]->spectateNext();
    }
}

void Character::getAvailableInteractions(std::vector<Interaction::Type> &fill)
{
    if(schema.health > 0)
    {
        Body::getAvailableInteractions(fill);
    }
    else
    {
        // Can loot bodies
        fill.push_back(Interaction::Type::OpenInventory);

        // Can consume bodies // TODO Maybe not for robots?
        fill.push_back(Interaction::Type::Consume);
    }
}

void Character::handleInteraction(Interaction::Type interaction, Body* interactor, bit::ServerPacket &responsePacket)
{
    Body::handleInteraction(interaction, interactor, responsePacket);

    switch (interaction)
    {
        case Interaction::Type::Consume:
        {
            // I am dead so I should be able to be destroyed

            // Give health to consumer
            // interactor body will probably be a character, and a player character
            if (interactor && interactor->schema.type == Body::Type::Character)
            {
                ::Character* character = static_cast<::Character*>(interactor);

                // TODO: Make this more reasonable (defined as a value of healing based on what is being consumed?)
                if (consumptionHeal > 0)
                    character->heal(consumptionHeal); // Heal for 10

                // Destroy this body
                level->removeCharacter(this); // THIS Maybe should be queued if it causes trouble inside of loops or ee

                Player* player = character->schema.player;
                if (player) {
                    level->sendEventToPlayer(player, [](bit::ServerPacket &packet) -> void {
                        packet << sf::Uint32(ServerEvent::CharacterConsumed);
                        // TODO: Could Send the Character::Type here so we could customize the client side effects
                    });
                }
            }


            // Success
            responsePacket << true;

            break;
        }
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

    // Active Effects
    packet << sf::Uint32(activeEffects.size());
    for (unsigned int i = 0; i < activeEffects.size(); i++)
    {
        packet << sf::Uint32(activeEffects[i]->type);
        packet << sf::Int32(activeEffects[i]->damageAmount);
        packet << sf::Uint32(activeEffects[i]->tileInterval);
        packet << sf::Uint32(activeEffects[i]->tileCounter);
        packet << sf::Uint32(activeEffects[i]->maxTiles);
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




///////////////////////////////////////////////////////
//                  FACTORIES                        //
///////////////////////////////////////////////////////

std::string Character::getTitle(Character::Type type)
{
    switch(type)
    {
        default:
            return "Undefined";
        case Character::Type::Zombie:
            return "Zombie";
        case Character::Type::Ogre:
            return "Ogre";
        case Character::Type::Hunter:
            return "Hunter";
        case Character::Type::Scientist:
            return "Scientist";
        case Character::Type::Guard:
            return "Guard";
        case Character::Type::Hazmaster:
            return "Hazmaster";
        case Character::Type::Batman:
            return "Man Bat";
        case Character::Type::Skeleton:
            return "Skeleton";
        case Character::Type::SkeletonLord:
            return "Bone Lord";
    }
}

std::string Character::getEquipmentSlotTitle(Character::EquipmentSlot slot)
{
    switch(slot)
    {
        default:
        case EquipmentSlot::_count:
            return "Undefined";
        case EquipmentSlot::Head:
            return "Head";
        case EquipmentSlot::Chest:
            return "Chest";
        case EquipmentSlot::Legs:
            return "Legs";
        case EquipmentSlot::Feet:
            return "Feet";
        case EquipmentSlot::Hands:
            return "Hands";
        case EquipmentSlot::Totem:
            return "Totem";
        case EquipmentSlot::WeaponPrimary:
            return "Primary Weapon";
        case EquipmentSlot::WeaponSecondary:
            return "Secondary Weapon";
    }
}