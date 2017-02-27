#include "AiRoutines.hpp"
#include "Character.hpp"
#include "Structure.hpp"
#include "Level.hpp"
#include "Tile.hpp"
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

bool AiRoutines::Combat::Hunter_DetectHostility(Character* character, Character* other)
{
    return (other->schema.type != Character::Type::Hunter);
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

            if(xDistance <= character->level->tileWidth * 2 && yDistance <= character->level->tileWidth * 2)
            {
                // If I am within 2 tiles move away from them
                int xDiff = x1 - x2;
                int yDiff = y1 - y2;

                // If i can move, do so, else im in a corner so attack
                if(character->canPathToPosition(x1 + xDiff, y1 + yDiff)) // TODO: Expensive
                {
                    character->combat_DecideAction_MoveToLocation(x1 + xDiff, y1 + yDiff);
                }
                else
                {
                    character->combat_DecideAction_AttackCharacter(closestEnemy);
                }
            }
            else if(xDistance <= character->level->tileWidth * 6 && yDistance <= character->level->tileWidth * 6)
            {
                // If they are further than 2 tiles but within 6 tiles attack them
                character->combat_DecideAction_AttackCharacter(closestEnemy);
            }
            else
            {
                // If they are more than 6 tiles away, TODO: Improve logic to move within range
                character->combat_DecideAction_Skip();
            }
        }
    }
}
