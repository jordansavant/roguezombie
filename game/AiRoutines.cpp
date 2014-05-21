#include "AiRoutines.hpp"
#include "Character.hpp"
#include "Level.hpp"
#include "Tile.hpp"
#include "../bitengine/Intelligence.hpp"

void AiRoutines::Combat::DecideZombie(Character* character)
{
    bool complete = false;
    character->inspectVisibleTiles([character, &complete] (Tile* tile) {

        if(complete)
            return;

        Body* body = tile->body;
        if(body && body->schema.type == Body::Type::Character)
        {
            Character* occupant = static_cast<Character*>(body);
            if(occupant != character)
            {
                // If the tile has a character that is not my own, choose them as my enemy and attack
                character->combat_DecideAction_AttackCharacter(occupant);
                complete = true;
            }
        }
    });

    // Lets decide we want to move 2 tiles north
    if(!complete)
        character->combat_DecideAction_MoveToLocation(character->Body::schema.x, character->Body::schema.y - character->level->tileHeight * 2);
}
