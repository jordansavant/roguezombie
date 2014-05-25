#pragma once
#ifndef RZ_COMMAND_H
#define RZ_COMMAND_H

#include "SFML/Network.hpp"
#include <functional>

class Command
{
public:

    Command();

    enum Type
    {
        None,
        PlayerMoveUp,
	    PlayerMoveDown,
	    PlayerMoveLeft,
	    PlayerMoveRight,
        PlayerDebug,
        CombatCommand,
        Free_MoveToTile,
        Combat_AttackCharacter,
    };

    enum CombatTarget
    {
        Tile,
        Character,
        Structure
    };

    enum TileTargetCommand
    {
        Move
    };

    enum CharacterTargetCommand
    {
        Attack
    };

    Type type;

    std::function<void(sf::Packet &packet)> pack;
};

#endif