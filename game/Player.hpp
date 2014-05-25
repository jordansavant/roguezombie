#pragma once
#ifndef RZ_PLAYER_H
#define RZ_PLAYER_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include "Command.hpp"

class Level;
class Character;

class Player
{
public:

    Player();

    Level* level;
	Character* character;
    unsigned int clientId;
    bit::RemoteClient* client;
    bool requestFullSnapshot;

    void load(bit::RemoteClient* client);

    void setLevel(Level* level);

    void setCharacter(Character* character);

    void handleCommand(bit::ClientPacket &packet, Command::Type commandType);

    bool validateCombat();
};

#endif