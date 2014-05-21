#include "AiRoutines.hpp"
#include "Character.hpp"
#include "Level.hpp"
#include "Tile.hpp"
#include "../bitengine/Intelligence.hpp"

void AiRoutines::Combat::DecideZombie(Character* character)
{
    bool complete = false;
    character->inspectVisibleCharacters([character, &complete] (Character* occupant) {
        if(complete)
            return;
        // If the tile has a character that is not my own, choose them as my enemy and attack
        character->combat_DecideAction_AttackCharacter(occupant);
        complete = true;
    });

    // Lets decide we want to move 2 tiles north
    if(!complete)
        character->combat_DecideAction_MoveToLocation(character->Body::schema.x, character->Body::schema.y - character->level->tileHeight * 2);
}
