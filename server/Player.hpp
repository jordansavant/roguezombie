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

    enum ControlState
    {
        Normal,
        Spectate
    };

    Level* level;
    ControlState controlState;
    Character* character;
    Character* spectatee;
    unsigned int clientId;
    bit::RemoteClient* client;
    bool requestFullSnapshot;

    void load(bit::RemoteClient* client);

    void setLevel(Level* level);

    void setCharacter(Character* character);

    void unsetCharacter();

    void setupPlayerCharacter();

    void onCharacterDeath(Character* character);

    void onLevelTransitionAttempt();

    void setSpectatee(Character* character);

    void unsetSpectatee();

    void spectateNext(int dir = 1);

    void handleCommand(bit::ClientPacket &packet, Command::Type commandType);

    bool validateFree();

    bool validateCombat();

    void onInteractionProcess();
};

#endif