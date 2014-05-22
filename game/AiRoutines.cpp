#include "AiRoutines.hpp"
#include "Character.hpp"
#include "Level.hpp"
#include "Tile.hpp"
#include "../bitengine/Intelligence.hpp"
#include "../bitengine/Math.hpp"

bool AiRoutines::Combat::Zombie_DetectHostility(Character* character, Character* other)
{
    return (other->schema.type != Character::Type::Zombie);
}

bool AiRoutines::Combat::Zombie_DetectCombat(Character* character)
{
    bit::Output::Debug(character->Body::schema.id);
    bool enemyDetected = false;
    character->inspectVisibleCharacters([character, &enemyDetected] (Character* occupant) {
        if(enemyDetected)
            return;
        enemyDetected = character->isHostileTowards(occupant);
    });

    return enemyDetected;
}

void AiRoutines::Combat::Zombie_DecideCombat(Character* character)
{
    // Find the closest enemy
    Character* closestEnemy = NULL;
    float closestDistance = 0;
    character->inspectVisibleCharacters([character, &closestEnemy, &closestDistance] (Character* occupant) {
        int x1 = character->Body::schema.x;
        int y1 = character->Body::schema.y;
        int width1 = character->Body::schema.width;
        int height1 = character->Body::schema.height;
        int x2 = occupant->Body::schema.x;
        int y2 = occupant->Body::schema.y;
        int width2 = occupant->Body::schema.width;
        int height2 = occupant->Body::schema.height;

        // If the occupant is not adjacent to me then move towards them
        float distance = bit::RectangleMath::distance(x1, y1, width1, height1, x2, y2, width2, height2);
        if(character->isHostileTowards(occupant) && (closestEnemy == NULL || distance < closestDistance))
        {
            closestEnemy = occupant;
            closestDistance = distance;
        }
    });

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
