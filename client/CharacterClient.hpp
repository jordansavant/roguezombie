#pragma once
#ifndef RZ_CHARACTERCLIENT_H
#define RZ_CHARACTERCLIENT_H

#include "BodyClient.hpp"
#include "../server/Character.hpp"
#include "../server/CharacterEffect.hpp"
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

    struct ActiveEffect
    {
        CharacterEffect::Type type;
        int damageAmount;
        unsigned int tileInterval;
        unsigned int tileCounter;
        unsigned int maxTiles;
    };

    CharacterClient();

    Character::Schema schema;
    CharacterSprite sprite;
    std::vector<bool> hasEquipment;
    std::vector<ItemClient> equipment;
    std::map<unsigned int, MissionClient> missionClients;
    float chanceOfHit;
    unsigned int visionRadius;
    std::vector<ActiveEffect> activeEffects;
    
    virtual void clientLoad(LevelClient* level);

    virtual void clientUpdate(sf::Time &gameTime);

    virtual void reinitialize();

    virtual void reset();

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);

    void handleServerEventPacket_missionCompleted(bit::ServerPacket &packet);

    void handleServerEventPacket_inventoryUpdate(bit::ServerPacket &packet);

    void handleServerEventPacket_itemAdded(bit::ServerPacket &packet);

    void handleServerEventPacket_itemUpdated(bit::ServerPacket &packet);

    void handleServerEventPacket_itemRemoved(bit::ServerPacket &packet);

    void handleServerEventPacket_equipmentAdded(bit::ServerPacket &packet);

    void handleServerEventPacket_equipmentUpdated(bit::ServerPacket &packet);

    void handleServerEventPacket_equipmentRemoved(bit::ServerPacket &packet);

    void handleServerEventPacket_equipmentSwapped(bit::ServerPacket &packet);

    void onEffectCreate(CharacterEffect::Type type);

    void onEffectRun(CharacterEffect::Type type);

    void onEffectRemove(CharacterEffect::Type type);

    virtual std::string getTitle();
};

#endif