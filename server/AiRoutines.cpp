#include "AiRoutines.hpp"
#include "Character.hpp"
#include "Structure.hpp"
#include "Level.hpp"
#include "Tile.hpp"
#include "items/Item.hpp"
#include "../bitengine/Intelligence.hpp"
#include "../bitengine/Math.hpp"

Tile* AiRoutines::Combat::findCoverPositionFromEnemy(Character* character, Character* enemy, unsigned int tileRadius)
{
    // Look at visible tiles within radius
    // If a tile has a non-blocking structure, consider it for inspection
    // Find opposing side of structure from enemy
    // If the opposing side is empty or myself return it as a cover position
    // If multiplle cover positions are viable then choose the one closest to me

    Tile* position = NULL;
    float closestDistance = 0;

    // Look at visible structures that are not walls (etc)
    character->inspectVisibleStructures([character, enemy, &position, &closestDistance] (Structure* structure) {

        // Get direction of enemy to obstacle
        sf::Vector2f direction = bit::VectorMath::directionToVector(enemy->Body::schema.x, enemy->Body::schema.y, structure->Body::schema.x, structure->Body::schema.y);

        // Find first tile in same direction (on other side)
        float oppositeSideX = structure->Body::schema.x + (direction.x * character->level->tileWidth);
        float oppositeSideY = structure->Body::schema.y + (direction.y * character->level->tileHeight);
        Tile* adjacentTile = character->level->getTileAtPosition(oppositeSideX + character->level->tileWidth / 2, oppositeSideY + character->level->tileHeight / 2);
        if(adjacentTile)
        {
            // If the tile has no body or the body is the same as me
            if(adjacentTile->body == NULL || adjacentTile->body->schema.id == character->Body::schema.id)
            {
                // Update my chosen position to the closest option
                float distance = bit::VectorMath::distance(character->Body::schema.x, character->Body::schema.y, adjacentTile->schema.x, adjacentTile->schema.y);
                if(position == NULL || distance < closestDistance)
                {
                    closestDistance = distance;
                    position = adjacentTile;
                }
            }
        }
    });

    return position;
}

bool AiRoutines::Combat::Default_DetectCombat(Character* character)
{
    bool enemyDetected = false;
    character->inspectVisibleCharacters([character, &enemyDetected] (Character* occupant) {
        if(enemyDetected)
            return;
        enemyDetected = character->isHostileTowards(occupant);
    });

    return enemyDetected;
}

bool AiRoutines::Combat::Zombie_DetectHostility(Character* character, Character* other)
{
    return (other->schema.type != Character::Type::Zombie);
}

void AiRoutines::Combat::Zombie_DecideCombat(Character* character)
{
    // Find the closest enemy
    Character* closestEnemy = character->getClosestVisibleEnemy();

    // If I have an enemy
    if(closestEnemy)
    {
        int x1 = character->Body::schema.x;
        int y1 = character->Body::schema.y;
        int width1 = character->Body::schema.width;
        int height1 = character->Body::schema.height;
        int x2 = closestEnemy->Body::schema.x;
        int y2 = closestEnemy->Body::schema.y;
        int width2 = closestEnemy->Body::schema.width;
        int height2 = closestEnemy->Body::schema.height;

        if(bit::RectangleMath::isCardinallyAdjacent(x1, y1, width1, height1, x2, y2, width2, height2))
        {
            // If I am directly next to them, attack them
            character->combat_DecideAction_AttackCharacter(closestEnemy);
        }
        else
        {
            // If they are not next to me then move towards them (DEBUG MOVE ONE TILE ABOVE)
            character->combat_DecideAction_MoveToLocation(closestEnemy->Body::schema.x, closestEnemy->Body::schema.y - character->level->tileHeight);
        }
    }
    // If I have no enemy, wander north
    else
    {
        character->combat_DecideAction_MoveToLocation(character->Body::schema.x, character->Body::schema.y - character->level->tileHeight * 2);
    }
}

bool AiRoutines::Combat::Generic_DetectHostility(Character* character, Character* other)
{
    return (other->schema.type == Character::Type::Zombie);
}

void AiRoutines::Combat::Hunter_DecideCombat(Character* character)
{
    // Predicates about Hunter
    // 1. I always carry a ranged weapon
    // 2. I am a 1x1 character

    // Find the closest enemy
    Character* closestEnemy = character->getClosestVisibleEnemy();

    // If I have an enemy
    if(closestEnemy)
    {
        // Priorities
        // - Finding cover between myself and my enemy
        // - If there is relevant cover
        //     - Find the opposite side of cover
        //     - If I am not on the opposite side
        //         - Move to cover position
        //     - If I am in the cover position
        //         - Attack closest enemy
        // - If there is no relevant cover
        //     - If the enemy is within X tiles
        //         - Move away from enemy to retain buffer distance
        //     - Else if they are further than X tiles
        //         - Fire toward the enemy

        // Locates a cover position that is: 1. between me and enemy, 2. empty or has me in it
        Tile* coverPosition = findCoverPositionFromEnemy(character, closestEnemy, 7); // 7 is the relevant search range
        
        if(coverPosition)
        {
            // If the cover position is empty
            if(coverPosition->body == NULL)
            {
                // Move toward cover position
                character->combat_DecideAction_MoveToLocation(coverPosition->schema.x, coverPosition->schema.y);
            }
            // If I am in cover
            else
            {
                // Attack my enemy
                character->combat_DecideAction_AttackCharacter(closestEnemy);
            }
        }
        else
        {
            int x1 = character->Body::schema.x;
            int y1 = character->Body::schema.y;
            int width1 = character->Body::schema.width;
            int height1 = character->Body::schema.height;

            int x2 = closestEnemy->Body::schema.x;
            int y2 = closestEnemy->Body::schema.y;
            int width2 = closestEnemy->Body::schema.width;
            int height2 = closestEnemy->Body::schema.height;

            int xDistance = bit::RectangleMath::axisDistance(bit::RectangleMath::Axis::X, x1, y1, width1, height1, x2, y2, width2, height2);
            int yDistance = bit::RectangleMath::axisDistance(bit::RectangleMath::Axis::Y, x1, y1, width1, height1, x2, y2, width2, height2);

            sf::Vector2f directionAway = bit::VectorMath::directionToVector(x2, y2, x1, y1);
            sf::Vector2f right(-directionAway.y, directionAway.x);
            sf::Vector2f left(directionAway.y, -directionAway.x);

            if(xDistance < character->level->tileWidth * 2 && yDistance < character->level->tileWidth * 2)
            {
                // If I am within 2 tiles, move away from them
                int tileDistance = 3;
                std::vector<sf::Vector2i> attempts;
                attempts.push_back(sf::Vector2i(x1 + directionAway.x * character->level->tileWidth * tileDistance, y1 + directionAway.y * character->level->tileHeight * tileDistance));
                attempts.push_back(sf::Vector2i(x1 + right.x * character->level->tileWidth * tileDistance, y1 + right.y * character->level->tileHeight * tileDistance));
                attempts.push_back(sf::Vector2i(x1 + left.x * character->level->tileWidth * tileDistance, y1 + left.y * character->level->tileHeight * tileDistance));

                for(unsigned int i=0; i < attempts.size(); i++)
                {
                    if(character->canPathToPosition(attempts[i].x, attempts[i].y))
                    {
                        // If this is a valid escape direction, take it
                        character->combat_DecideAction_MoveToLocation(attempts[i].x, attempts[i].y);
                        break;
                    }
                    else if(i == attempts.size() - 1)
                    {
                        // Otherwise if no directions remain, attack character
                        character->combat_DecideAction_AttackCharacter(closestEnemy);
                    }
                }
            }
            else if(xDistance < character->level->tileWidth * 6 && yDistance < character->level->tileWidth * 6)
            {
                // If they are further than 2 tiles but within 6 tiles attack them
                character->combat_DecideAction_AttackCharacter(closestEnemy);
            }
            else
            {
                // If I am further than 6 tiles, move toward them, trying longer distances up until shorter distances
                std::vector<sf::Vector2i> attempts;
                attempts.push_back(sf::Vector2i(x1 - directionAway.x * character->level->tileWidth * 4, y1 - directionAway.y * character->level->tileHeight * 4));
                attempts.push_back(sf::Vector2i(x1 - directionAway.x * character->level->tileWidth * 2, y1 - directionAway.y * character->level->tileHeight * 2));
                attempts.push_back(sf::Vector2i(x1 - directionAway.x * character->level->tileWidth * 1, y1 - directionAway.y * character->level->tileHeight * 1));

                for(unsigned int i=0; i < attempts.size(); i++)
                {
                    if(character->canPathToPosition(attempts[i].x, attempts[i].y))
                    {
                        // If this is a valid chase direction, take it
                        character->combat_DecideAction_MoveToLocation(attempts[i].x, attempts[i].y);
                        break;
                    }
                    else if(i == attempts.size() - 1)
                    {
                        // Otherwise if no directions remain, attack character
                        character->combat_DecideAction_AttackCharacter(closestEnemy);
                    }
                }
            }
        }
    }
    else
    {
        character->combat_DecideAction_Skip();
    }
}

void AiRoutines::Combat::Guard_DecideCombat(Character* character)
{
    // Predicates about Guard
    // 1. I either carry a range weapon or a melee weapon
    // 2. I am a 1x1 character

    // Guards should roughly act like hunters but change their action based on their weapon type
    // If they are using a weapon that has a range <= 2 they should just run to the character and attack

    // Figure out range of weapon
    unsigned int attackRange = 1;
    bool isMelee = true;
    Item* weapon = character->equipment[Character::EquipmentSlot::WeaponPrimary];
    if (weapon)
    {
        attackRange = weapon->schema.effectiveRangeInTiles;
        isMelee = weapon->schema.isOfWeaponType(ItemCategory::Weapon::WeaponMelee);
    }

    // Find closest enemy and get our distances apart
    Character* closestEnemy = character->getClosestVisibleEnemy();
    if (closestEnemy)
    {
        bool seekCover = false;
        int x1 = character->Body::schema.x;
        int y1 = character->Body::schema.y;
        int width1 = character->Body::schema.width;
        int height1 = character->Body::schema.height;

        int x2 = closestEnemy->Body::schema.x;
        int y2 = closestEnemy->Body::schema.y;
        int width2 = closestEnemy->Body::schema.width;
        int height2 = closestEnemy->Body::schema.height;

        int xDistance = bit::RectangleMath::axisDistance(bit::RectangleMath::Axis::X, x1, y1, width1, height1, x2, y2, width2, height2);
        int yDistance = bit::RectangleMath::axisDistance(bit::RectangleMath::Axis::Y, x1, y1, width1, height1, x2, y2, width2, height2);
        xDistance = std::abs(x2 - x1);
        yDistance = std::abs(y2 - y1);

        sf::Vector2f directionAway = bit::VectorMath::directionToVector(x2, y2, x1, y1);
        sf::Vector2f directionToward = bit::VectorMath::directionToVector(x1, y1, x2, y2);
        sf::Vector2f right(-directionAway.y, directionAway.x);
        sf::Vector2f left(directionAway.y, -directionAway.x);

        // If I am using a melee weapon go straight toward the enemy
        if (isMelee)
        {
            if (xDistance <= character->level->tileWidth * attackRange && yDistance <= character->level->tileWidth * attackRange)
            {
                // If I am within melee attack range, then attack them
                character->combat_DecideAction_AttackCharacter(closestEnemy);
            }
            else
            {
                // Go straight toward them
                // Look at their adjacent tiles and find an open one nearby and set it as our destination
                std::vector<sf::Vector2i> attempts;
                attempts.push_back(sf::Vector2i(x2 + directionAway.x * character->level->tileWidth * 1, y2 + directionAway.y * character->level->tileHeight * 1)); // tile nearest me
                attempts.push_back(sf::Vector2i(x2 + right.x * character->level->tileWidth * 2, y2 + right.y * character->level->tileHeight * 2)); // tile to right of them
                attempts.push_back(sf::Vector2i(x2 + left.x * character->level->tileWidth * 1, y2 + left.y * character->level->tileHeight * 1)); // tile to left of them
                attempts.push_back(sf::Vector2i(x2 + directionToward.x * character->level->tileWidth * 1, y2 + directionToward.y * character->level->tileHeight * 1)); // tile behind them

                for (unsigned int i = 0; i < attempts.size(); i++)
                {
                    if (character->canPathToPosition(attempts[i].x, attempts[i].y))
                    {
                        // If this is a place, take it
                        character->combat_DecideAction_MoveToLocation(attempts[i].x, attempts[i].y);
                        break;
                    }
                    else if (i == attempts.size() - 1)
                    {
                        // Otherwise if no directions remain, attack character
                        seekCover = true;
                        character->combat_DecideAction_AttackCharacter(closestEnemy);
                    }
                }

            }
        }
        else
        {
            // If I am using a ranged weapon look for cover and adaptively attack
            seekCover = true;
        }


        if (seekCover)
        {
            // Priorities
            // - Finding cover between myself and my enemy
            // - If there is relevant cover
            //     - Find the opposite side of cover
            //     - If I am not on the opposite side
            //         - Move to cover position
            //     - If I am in the cover position
            //         - Attack closest enemy
            // - If there is no relevant cover
            //     - If the enemy is within X tiles
            //         - Move away from enemy to retain buffer distance
            //     - Else if they are further than X tiles
            //         - Fire toward the enemy

            // Locates a cover position that is: 1. between me and enemy, 2. empty or has me in it
            Tile* coverPosition = findCoverPositionFromEnemy(character, closestEnemy, 7); // 7 is the relevant search range

            if (coverPosition)
            {
                // If the cover position is empty
                if (coverPosition->body == NULL)
                {
                    // Move toward cover position
                    character->combat_DecideAction_MoveToLocation(coverPosition->schema.x, coverPosition->schema.y);
                }
                // If I am in cover
                else
                {
                    // Attack my enemy
                    character->combat_DecideAction_AttackCharacter(closestEnemy);
                }
            }
            else
            {
                if (xDistance < character->level->tileWidth * 2 && yDistance < character->level->tileWidth * 2)
                {
                    // If I am within 2 tiles, move away from them
                    int tileDistance = 3;
                    std::vector<sf::Vector2i> attempts;
                    attempts.push_back(sf::Vector2i(x1 + directionAway.x * character->level->tileWidth * tileDistance, y1 + directionAway.y * character->level->tileHeight * tileDistance));
                    attempts.push_back(sf::Vector2i(x1 + right.x * character->level->tileWidth * tileDistance, y1 + right.y * character->level->tileHeight * tileDistance));
                    attempts.push_back(sf::Vector2i(x1 + left.x * character->level->tileWidth * tileDistance, y1 + left.y * character->level->tileHeight * tileDistance));

                    for (unsigned int i = 0; i < attempts.size(); i++)
                    {
                        if (character->canPathToPosition(attempts[i].x, attempts[i].y))
                        {
                            // If this is a valid escape direction, take it
                            character->combat_DecideAction_MoveToLocation(attempts[i].x, attempts[i].y);
                            break;
                        }
                        else if (i == attempts.size() - 1)
                        {
                            // Otherwise if no directions remain, attack character
                            character->combat_DecideAction_AttackCharacter(closestEnemy);
                        }
                    }
                }
                else if (xDistance < character->level->tileWidth * 6 && yDistance < character->level->tileWidth * 6)
                {
                    // If they are further than 2 tiles but within 6 tiles attack them
                    character->combat_DecideAction_AttackCharacter(closestEnemy);
                }
                else
                {
                    // If I am further than 6 tiles, move toward them, trying longer distances up until shorter distances
                    std::vector<sf::Vector2i> attempts;
                    attempts.push_back(sf::Vector2i(x1 - directionAway.x * character->level->tileWidth * 4, y1 - directionAway.y * character->level->tileHeight * 4));
                    attempts.push_back(sf::Vector2i(x1 - directionAway.x * character->level->tileWidth * 2, y1 - directionAway.y * character->level->tileHeight * 2));
                    attempts.push_back(sf::Vector2i(x1 - directionAway.x * character->level->tileWidth * 1, y1 - directionAway.y * character->level->tileHeight * 1));

                    for (unsigned int i = 0; i < attempts.size(); i++)
                    {
                        if (character->canPathToPosition(attempts[i].x, attempts[i].y))
                        {
                            // If this is a valid chase direction, take it
                            character->combat_DecideAction_MoveToLocation(attempts[i].x, attempts[i].y);
                            break;
                        }
                        else if (i == attempts.size() - 1)
                        {
                            // Otherwise if no directions remain, attack character
                            character->combat_DecideAction_AttackCharacter(closestEnemy);
                        }
                    }
                }
            }
        }
    }
    else
    {
        character->combat_DecideAction_Skip();
    }
}


void AiRoutines::Combat::Coverless_DecideCombat(Character* character)
{
    // Characters with coverless ai will just make sure they are within an effective range of their weapon

    Item* weapon = character->equipment[Character::EquipmentSlot::WeaponPrimary];
    unsigned int attackRange = weapon ? weapon->schema.effectiveRangeInTiles : 1;

    // Find closest enemy and get our distances apart
    Character* closestEnemy = character->getClosestVisibleEnemy();
    if (closestEnemy)
    {
        int x1 = character->Body::schema.x;
        int y1 = character->Body::schema.y;
        int width1 = character->Body::schema.width;
        int height1 = character->Body::schema.height;

        int x2 = closestEnemy->Body::schema.x;
        int y2 = closestEnemy->Body::schema.y;
        int width2 = closestEnemy->Body::schema.width;
        int height2 = closestEnemy->Body::schema.height;

        int xDistance = std::abs(x2 - x1);
        int yDistance = std::abs(y2 - y1);

        sf::Vector2f directionAway = bit::VectorMath::directionToVector(x2, y2, x1, y1);
        sf::Vector2f directionToward = bit::VectorMath::directionToVector(x1, y1, x2, y2);
        sf::Vector2f right(-directionAway.y, directionAway.x);
        sf::Vector2f left(directionAway.y, -directionAway.x);

        // If I am using a melee weapon go straight toward the enemy
        if (xDistance <= character->level->tileWidth * attackRange && yDistance <= character->level->tileWidth * attackRange)
        {
            // If I am within attack range, then attack them
            character->combat_DecideAction_AttackCharacter(closestEnemy);
        }
        else
        {
            // Go straight toward them
            // Look at their adjacent tiles and find an open one nearby and set it as our destination
            std::vector<sf::Vector2i> attempts;
            attempts.push_back(sf::Vector2i(x2 + directionAway.x * character->level->tileWidth * 1, y2 + directionAway.y * character->level->tileHeight * 1)); // tile nearest me
            attempts.push_back(sf::Vector2i(x2 + right.x * character->level->tileWidth * 2, y2 + right.y * character->level->tileHeight * 2)); // tile to right of them
            attempts.push_back(sf::Vector2i(x2 + left.x * character->level->tileWidth * 1, y2 + left.y * character->level->tileHeight * 1)); // tile to left of them
            attempts.push_back(sf::Vector2i(x2 + directionToward.x * character->level->tileWidth * 1, y2 + directionToward.y * character->level->tileHeight * 1)); // tile behind them

            for (unsigned int i = 0; i < attempts.size(); i++)
            {
                if (character->canPathToPosition(attempts[i].x, attempts[i].y))
                {
                    // If this is a place, take it
                    character->combat_DecideAction_MoveToLocation(attempts[i].x, attempts[i].y);
                    break;
                }
                else if (i == attempts.size() - 1)
                {
                    // Otherwise if no directions remain, attack character
                    character->combat_DecideAction_AttackCharacter(closestEnemy);
                }
            }
        }
    }
    else
    {
        character->combat_DecideAction_Skip();
    }
}

void AiRoutines::Combat::Scientist_DecideCombat(Character* character)
{
    // Predicates about Scientist
    // 1. I always carry no weapons
    // 2. I am a 1x1 character
    // 3. I only attack in self defense of melee range

    // Find the closest enemy
    Character* closestEnemy = character->getClosestVisibleEnemy();

    // If I have an enemy
    if (closestEnemy)
    {
        // Priorities
        // - Finding cover between myself and my enemy
        // - If there is relevant cover
        //     - Find the opposite side of cover
        //     - If I am not on the opposite side
        //         - Move to cover position
        //     - If I am in the cover position
        //         - Attack closest enemy
        // - If there is no relevant cover
        //     - If the enemy is within X tiles
        //         - Move away from enemy to retain buffer distance
        //     - Else if they are further than X tiles
        //         - Fire toward the enemy

        // Locates a cover position that is: 1. between me and enemy, 2. empty or has me in it
        Tile* coverPosition = findCoverPositionFromEnemy(character, closestEnemy, 7); // 7 is the relevant search range

        if (coverPosition)
        {
            // If the cover position is empty
            if (coverPosition->body == NULL)
            {
                // Move toward cover position
                character->combat_DecideAction_MoveToLocation(coverPosition->schema.x, coverPosition->schema.y);
            }
            // If I am in cover
            else
            {
                // Skip and hide
                character->combat_DecideAction_Skip();
            }
        }
        else
        {
            int x1 = character->Body::schema.x;
            int y1 = character->Body::schema.y;
            int width1 = character->Body::schema.width;
            int height1 = character->Body::schema.height;

            int x2 = closestEnemy->Body::schema.x;
            int y2 = closestEnemy->Body::schema.y;
            int width2 = closestEnemy->Body::schema.width;
            int height2 = closestEnemy->Body::schema.height;

            int xDistance = bit::RectangleMath::axisDistance(bit::RectangleMath::Axis::X, x1, y1, width1, height1, x2, y2, width2, height2);
            int yDistance = bit::RectangleMath::axisDistance(bit::RectangleMath::Axis::Y, x1, y1, width1, height1, x2, y2, width2, height2);

            sf::Vector2f directionAway = bit::VectorMath::directionToVector(x2, y2, x1, y1);
            sf::Vector2f right(-directionAway.y, directionAway.x);
            sf::Vector2f left(directionAway.y, -directionAway.x);

            if (xDistance < character->level->tileWidth * 2 && yDistance < character->level->tileWidth * 2)
            {
                // If they less than 2 tile away try and defend myself
                character->combat_DecideAction_AttackCharacter(closestEnemy);
            }
            else if (xDistance < character->level->tileWidth * 6 && yDistance < character->level->tileWidth * 6)
            {
                // If I am within 6 tiles, move away from them
                int tileDistance = 3;
                std::vector<sf::Vector2i> attempts;
                attempts.push_back(sf::Vector2i(x1 + directionAway.x * character->level->tileWidth * tileDistance, y1 + directionAway.y * character->level->tileHeight * tileDistance));
                attempts.push_back(sf::Vector2i(x1 + right.x * character->level->tileWidth * tileDistance, y1 + right.y * character->level->tileHeight * tileDistance));
                attempts.push_back(sf::Vector2i(x1 + left.x * character->level->tileWidth * tileDistance, y1 + left.y * character->level->tileHeight * tileDistance));

                for (unsigned int i = 0; i < attempts.size(); i++)
                {
                    if (character->canPathToPosition(attempts[i].x, attempts[i].y))
                    {
                        // If this is a valid escape direction, take it
                        character->combat_DecideAction_MoveToLocation(attempts[i].x, attempts[i].y);
                        break;
                    }
                    else if (i == attempts.size() - 1)
                    {
                        // Otherwise if no directions remain, attack character
                        character->combat_DecideAction_Skip();
                    }
                }
            }
            else
            {
                // If I am greater than 6 tiles away, move randomly one tile
                character->combat_DecideAction_MoveToLocation(character->Body::schema.x + directionAway.x * character->level->tileWidth, character->Body::schema.y + directionAway.y * character->level->tileHeight);
            }
        }
    }
    else
    {
        character->combat_DecideAction_Skip();
    }
}


void AiRoutines::Combat::Savage_DecideCombat(Character* character)
{
    // SAVAGE COMBAT AI
    // A rudimentary AI where the character approaches enemies to within their effective range
    // 1. Will look for any visible enemies and target the closest one
    // 2. Will check effective range of weapon (or unarmed)
    // 3. If within that range already, will attack
    // 4. If not within that range will look for a location within that range and move to it

    // Get range
    unsigned int attackRange = character->getEffectiveRange();
    float attackRangeDistance = character->level->tileWidth * attackRange;
    unsigned int maxAttackRange = character->getEffectiveRange();
    float maxAttackRangeDistance = character->level->tileWidth * attackRange;

    // Find closest enemy and get our distances apart
    Character* closestEnemy = character->getClosestVisibleEnemy();
    if (closestEnemy)
    {
        float x1 = character->Body::schema.x;
        float y1 = character->Body::schema.y;
        float x2 = closestEnemy->Body::schema.x;
        float y2 = closestEnemy->Body::schema.y;

        float distance = bit::VectorMath::distance(x1, y1, x2, y2);
        int xDistance = std::abs(x2 - x1);
        int yDistance = std::abs(y2 - y1);

        bool withinEffectiveRange = distance <= attackRangeDistance;
        bool withinMaximumRange = distance <= maxAttackRangeDistance;

        // Am I within range of this enemy?
        if (withinMaximumRange)
        {
            // Attack the enemy
            character->combat_DecideAction_AttackCharacter(closestEnemy);
        }
        else
        {
            // Look for a spot nearby we can get to
            findOpenAdjacentPosition(character, closestEnemy,
                [] (Character* character, Character* enemy, sf::Vector2i pos) -> void {
                    // If a spot exists then lets path toward it
                    character->combat_DecideAction_MoveToLocation(pos.x, pos.y);
                },
                [] (Character* character, Character* enemy) -> void {
                    // If no spot exists, yet we are out of range
                    // If I have a weapon that 
                    character->combat_DecideAction_AttackCharacter(enemy);
                }
            );
        }
    }
    else
    {
        character->combat_DecideAction_Skip();
    }
}

void AiRoutines::Combat::findOpenAdjacentPosition(Character* character, Character* enemy, std::function<void(Character* character, Character* enemy, sf::Vector2i pos)> onSuccess, std::function<void(Character* character, Character* enemy)> onFailure)
{
    float x1 = character->Body::schema.x;
    float y1 = character->Body::schema.y;
    float x2 = enemy->Body::schema.x;
    float y2 = enemy->Body::schema.y;

    // Look for a spot within range of the enemy
    sf::Vector2f directionAway = bit::VectorMath::directionToVector(x2, y2, x1, y1);
    sf::Vector2f directionToward = bit::VectorMath::directionToVector(x1, y1, x2, y2);
    sf::Vector2f right(-directionAway.y, directionAway.x);
    sf::Vector2f left(directionAway.y, -directionAway.x);

    std::vector<sf::Vector2i> attempts;
    attempts.push_back(sf::Vector2i(x2 + directionAway.x * character->level->tileWidth * 1, y2 + directionAway.y * character->level->tileHeight * 1)); // tile nearest me
    attempts.push_back(sf::Vector2i(x2 + right.x * character->level->tileWidth * 2, y2 + right.y * character->level->tileHeight * 2)); // tile to right of them
    attempts.push_back(sf::Vector2i(x2 + left.x * character->level->tileWidth * 1, y2 + left.y * character->level->tileHeight * 1)); // tile to left of them
    attempts.push_back(sf::Vector2i(x2 + directionToward.x * character->level->tileWidth * 1, y2 + directionToward.y * character->level->tileHeight * 1)); // tile behind them

    for (unsigned int i = 0; i < attempts.size(); i++)
    {
        if (character->canPathToPosition(attempts[i].x, attempts[i].y))
        {
            // If this place is reachable, lets do it
            onSuccess(character, enemy, sf::Vector2i(attempts[i].x, attempts[i].y));
            return;
        }
    }

    onFailure(character, enemy);
}