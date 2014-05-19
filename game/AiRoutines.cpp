#include "AiRoutines.hpp"
#include "Character.hpp"
#include "Level.hpp"

void AiRoutines::Combat::DecideZombie(Character* character)
{
    // Lets decide we want to move 2 tiles north
    character->combat_DecideAction_MoveToLocation(character->Body::schema.x, character->Body::schema.y - character->level->tileHeight * 2);
}