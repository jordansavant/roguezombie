#pragma once
#ifndef RZ_CHARACTERCLIENT_H
#define RZ_CHARACTERCLIENT_H

#include "BodyClient.hpp"
#include "Character.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include <map>
#include "mission/MissionClient.hpp"
#include "items/ItemClient.hpp"

class CharacterClient : public BodyClient
{
public:

    CharacterClient();

	Character::Schema schema;
    std::map<unsigned int, MissionClient> missionClients;
    ItemClient backpackClient;

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);

    void handleServerEventPacket_missionCompleted(bit::ServerPacket &packet);

    void handleServerEventPacket_itemAdded(bit::ServerPacket &packet);
};

#endif