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
        PlayerClickTile,
        PlayerRightClickTile,
    };
    Type type;

    std::function<void(sf::Packet &packet)> pack;
};

#endif