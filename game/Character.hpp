#pragma once
#ifndef RZ_CHARACTER_H
#define RZ_CHARACTER_H

#include "Body.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include <map>

#include "mission/MissionClient.hpp"
#include "items/ItemClient.hpp"

class Level;
class Player;
class Light;
class Tile;
class Mission;
class Item;

class Character : public Body
{
public:

    Character();

    virtual ~Character();

    enum Type
    {
       Zombie,
       Ogre
    };

    std::vector<Tile*> path;
    bit::GameTimer moveTimer;
    std::vector<Light*> lights;
    std::vector<Mission*> missions;
    Item* backpack;

    std::map<unsigned int, MissionClient> missionClients; // TODO: Remove clientside aspect from serverside class
    ItemClient backpackClient; // TODO: Remove clientside aspect from serverside class

	struct FixedState
	{
		int maxHealth;
        bool isPlayerCharacter;
        unsigned int clientId;
        Player* player;
        Type type;

        friend sf::Packet& operator <<(sf::Packet& packet, const Character::FixedState &state)
        {
            packet << sf::Uint32(state.maxHealth);
            packet << state.isPlayerCharacter;
            packet << sf::Uint32(state.clientId);
            packet << sf::Uint32(state.type);
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Character::FixedState &state)
        {
            packet >> state.maxHealth;
            packet >> state.isPlayerCharacter;
            packet >> state.clientId;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Character::Type>(packet, state.type);
            return packet;
        }
	};
	FixedState fixedState;

	struct DeltaState
	{
        int health;

        friend sf::Packet& operator <<(sf::Packet& packet, const Character::DeltaState &state)
        {
            return packet << sf::Int32(state.health);
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Character::DeltaState &state)
        {
            return packet >> state.health;
        }
	};
	DeltaState deltaState;

    virtual void load(Level* level, unsigned int id, Type type, float x, float y, float width, float height);

    virtual void update(sf::Time &gameTime);

    virtual void setControllingPlayer(Player* player);

    virtual bool moveUp();

    virtual bool moveDown();

    virtual bool moveLeft();

    virtual bool moveRight();

    virtual void pathToPosition(float x, float y);

    virtual bool moveToTile(Tile* tile);

    virtual bool moveToPosition(float x, float y);

    void assignMission(Mission* mission);

    void addItemToInventory(Item* item);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

    virtual void handleSnapshot(bit::ServerPacket &packet, bool full = false);

    void handleMissionCompleteGameEvent(bit::ServerPacket &packet);

    void handleItemAddGameEvent(bit::ServerPacket &packet);

protected:

    virtual bool isTileBlocked(Tile* tile);

    virtual bool isTileBlockedForPathfinding(Tile* tile);

    void checkMissions();
};

#endif