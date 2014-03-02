#pragma once
#ifndef RZ_COMMAND_H
#define RZ_COMMAND_H

#include "SFML/Network.hpp"

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
        PlayerTeleport,
    };
    Type type;

    sf::Packet data;
};

#endif