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
        FreeCommand,
        ItemCommand,
        Combat_AttackCharacter,
		Dev_KillAll
    };

    enum Target
    {
        NoTarget,
        Tile,
        Character,
        Structure
    };

    enum NonTargetCommand
    {
        Skip,
        SwapWeapon
    };

    enum TargetTileCommand
    {
        Move
    };

    enum TargetCharacterCommand
    {
        Attack
    };

    Type type;

    std::function<void(sf::Packet &packet)> pack;
};

#endif