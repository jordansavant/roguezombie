#pragma once
#ifndef RZ_CHARACTERCLIENT_H
#define RZ_CHARACTERCLIENT_H

#include "BodyClient.hpp"
#include "Character.hpp"
#include "CharacterSprite.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include <map>
#include "mission/MissionClient.hpp"

class CharacterClient : public BodyClient
{
public:

    CharacterClient();

	Character::Schema schema;
    CharacterSprite sprite;

    std::vector<bool> hasEquipment;
    std::vector<ItemClient> equipment;
    
    std::map<unsigned int, MissionClient> missionClients;

    virtual void reinitialize();

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);

    void handleServerEventPacket_missionCompleted(bit::ServerPacket &packet);

    void handleServerEventPacket_itemAdded(bit::ServerPacket &packet);

    void handleServerEventPacket_itemRemoved(bit::ServerPacket &packet);
};

#endif