#pragma once
#ifndef RZ_CHARACTER_H
#define RZ_CHARACTER_H

#include "Body.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"

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
        None,
        Zombie,
        Ogre
    };

    std::vector<Tile*> path;
    bit::GameTimer moveTimer;
    std::vector<Light*> lights;
    std::vector<Mission*> missions;
    Item* equipmentSlot_head;
    //Item* equipmentSlot_chest;
    //Item* equipmentSlot_legs;
    //Item* equipmentSlot_feet;
    //Item* equipmentSlot_hands;
    //Item* equipmentSlot_totem;
    //Item* equipmentSlot_weaponPrimary;
    //Item* equipmentSlot_weaponSecondary;

	struct Schema
	{
        Schema()
           : maxHealth(0), isPlayerCharacter(false), clientId(0), player(NULL), type(Type::None), health(0), itemId_equipmentSlot_head(0)
        {
        }

		int maxHealth;
        bool isPlayerCharacter;
        unsigned int clientId;
        Player* player;
        Type type;
        int health;
        unsigned int itemId_equipmentSlot_head;

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            packet << sf::Uint32(schema.maxHealth);
            packet << schema.isPlayerCharacter;
            packet << sf::Uint32(schema.clientId);
            packet << sf::Uint32(schema.type);
            packet << sf::Int32(schema.health);
            packet << sf::Uint32(schema.itemId_equipmentSlot_head);
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            packet >> schema.maxHealth;
            packet >> schema.isPlayerCharacter;
            packet >> schema.clientId;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Character::Type>(packet, schema.type);
            packet >> schema.health;
            packet >> schema.itemId_equipmentSlot_head;
            return packet;
        }
	};
	Schema schema;

    virtual void load(Level* level, unsigned int id, Type type, float x, float y, float width, float height);

    virtual void update(sf::Time &gameTime);

    virtual bool equipHeadFromInventory(unsigned int itemId);

    virtual bool equipHead(Item* item);

    virtual void unequipHead();

    virtual void setControllingPlayer(Player* player);

    virtual bool moveUp();

    virtual bool moveDown();

    virtual bool moveLeft();

    virtual bool moveRight();

    virtual void pathToPosition(float x, float y);

    virtual bool moveToTile(Tile* tile);

    virtual bool moveToPosition(float x, float y);

    void assignMission(Mission* mission);

    virtual void prepareSnapshot(bit::ServerPacket &packet, bool full = false);

protected:

    virtual bool isTileBlocked(Tile* tile);

    virtual bool isTileBlockedForPathfinding(Tile* tile);

    void checkMissions();
};

#endif