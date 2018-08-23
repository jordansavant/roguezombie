#pragma once
#ifndef RZ_CHARACTER_H
#define RZ_CHARACTER_H

#include "Body.hpp"
#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Structures.hpp"

class Level;
class Player;
class Tile;
class Mission;
class Item;
class Structure;

class Character : public Body
{
public:

    Character();

    virtual ~Character();

    enum Type
    {
        None,
        Zombie,
        Ogre,
        Hunter,
        Scientist,
        Guard
    };

    enum EquipmentSlot
    {
        Head,
        Chest,
        Legs,
        Feet,
        Hands,
        Totem,
        WeaponPrimary,
        WeaponSecondary,
        _count,
    };

    enum CombatState
    {
        Waiting,
        DecideAction,
        PerformAction,
        Delay,
    };

    enum CombatAction
    {
        Move,
        Attack,
        Skip,
        SwapWeapon,
        UsedItem
    };

    CombatState combatState;
    CombatAction combatAction;
    bit::GameTimer actionDelayTimer;
    std::function<bool(Character*, Character*)> hostilityCheckAi;
    std::function<bool(Character*)> combatDetectionAi;
    std::function<void(Character*)> combatDecisionAi;
    bool isHostileCombatDetected;
    unsigned int combatTilesTraversed;
    std::vector<Tile*> path;
    bool hasTargetEnemy;
    sf::Vector2f targetEnemyPosition;
    bit::GameTimer moveTimer;
    std::vector<Mission*> missions;
    std::vector<Item*> equipment;
    unsigned int visionRadius;
    std::vector<Player*> spectators;
    unsigned int consumptionHeal;

    // Events
    bit::Event<std::function<void(Character* c)>> onDeath;

    struct Schema
    {
        Schema()
            : isPlayerCharacter(false), clientId(0), player(NULL), type(Type::None), direction(0, 0),
              maxActionPoints(2), currentActionPoints(2), maxHealth(0), health(0), speed(3),
              intelligence(1), dexterity(1), strength(1)
        {
            equipmentIds.resize(EquipmentSlot::_count, 0);
        }

        // Metadata
        bool isPlayerCharacter;
        unsigned int clientId;
        Player* player;
        Type type;
        std::vector<unsigned int> equipmentIds;
        sf::Vector2f direction;

        // Stats
        int maxActionPoints;
        int currentActionPoints;
        int maxHealth;
        int health;
        int speed; // tiles per action point
        int intelligence;
        int strength;
        int dexterity;

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            // Metadata
            packet << schema.isPlayerCharacter;
            packet << sf::Uint32(schema.clientId);
            packet << sf::Uint32(schema.type);
            for(unsigned int i=0; i < schema.equipmentIds.size(); i++)
            {
                packet << sf::Uint32(schema.equipmentIds[i]);
            }
            packet << schema.direction.x;
            packet << schema.direction.y;

            // Stats
            packet << sf::Int32(schema.maxActionPoints);
            packet << sf::Int32(schema.currentActionPoints);
            packet << sf::Int32(schema.maxHealth);
            packet << sf::Int32(schema.health);
            packet << sf::Int32(schema.speed);
            if(schema.isPlayerCharacter)
            {
                packet << sf::Int32(schema.intelligence);
                packet << sf::Int32(schema.dexterity);
                packet << sf::Int32(schema.strength);
            }
            return packet;
        }
        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            // Metadata
            packet >> schema.isPlayerCharacter;
            packet >> schema.clientId;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Character::Type>(packet, schema.type);
            for(unsigned int i=0; i < Character::EquipmentSlot::_count; i++)
            {
                packet >> schema.equipmentIds[i];
            }
            packet >> schema.direction.x;
            packet >> schema.direction.y;

            // Stats
            packet >> schema.maxActionPoints;
            packet >> schema.currentActionPoints;
            packet >> schema.maxHealth;
            packet >> schema.health;
            packet >> schema.speed;
            if(schema.isPlayerCharacter)
            {
                packet >> schema.intelligence;
                packet >> schema.dexterity;
                packet >> schema.strength;
            }
            return packet;
        }

        bool isDead();
    };
    Schema schema;


    // Game Loop

    virtual void load(Level* level, unsigned int id, Type type, float x, float y, float width, float height);

    virtual void update(sf::Time &gameTime);

    virtual void updateAlive(sf::Time &gameTime);

    virtual void updateDead(sf::Time &gameTime);

    virtual void updateAlive_Free(sf::Time &gameTime);

    virtual void updateAlive_Combat(sf::Time &gameTime);

    virtual void combat_DecideAction(sf::Time &gameTime);

    virtual void combat_DecideAction_MoveToLocation(int x, int y);

    virtual void combat_PerformAction_MoveToLocation(sf::Time &gameTime);

    virtual void combat_DecideAction_AttackCharacter(Character* character);

    virtual void combat_PerformAction_AttackCharacter(sf::Time &gameTime);

    virtual void combat_DecideAction_Skip();

    virtual void combat_PerformAction_Skip(sf::Time &gameTime);

    virtual void combat_DecideAction_SwapWeapon();

    virtual void combat_PerformAction_SwapWeapon(sf::Time &gameTime);

    virtual void combat_DecideAction_UsedItem();

    virtual void combat_PerformAction_UsedItem(sf::Time &gameTime);


    void combat_SwitchStateWaiting();

    void combat_SwitchStateDecide();

    void combat_SwitchStatePerform();
     
    void combat_SwitchStateDelay();

    virtual void distributedUpdate(sf::Time &gameTime);

    virtual void distributedUpdateAlive(sf::Time &gameTime);

    virtual void distributedUpdateDead(sf::Time &gameTime);

    virtual void distributedUpdateAlive_Free(sf::Time &gameTime);

    virtual void distributedUpdateAlive_Combat(sf::Time &gameTime);


    // Character actions

    virtual bool isHostileTowards(Character* character);

    virtual void followPath(sf::Time &gameTime);

    virtual void kill();

    virtual void attack(Character* character);

    virtual void heal(int amount);

    virtual void harm(int damage);

    virtual void detectCombatEnter();

    virtual void detectCombatExit();

    virtual bool canDodge();

    virtual bool canBlock();


    // Inspection

    void inspectVisibleTiles(std::function<void(Tile* t)> inspector);

    void inspectVisibleCharacters(std::function<void(Character* c)> inspector, bool onlyLiving = true);

    void inspectVisibleStructures(std::function<void(::Structure* s)> inspector, bool onlyNonBlocking = true);

    void inspectCombatReachableTiles(std::function<void(Tile*)> inspector);

    bool inspectTileVisuallyBlocked(int x, int y);

    bool isTileVisuallyBlocked(Tile* t);

    bool inspectTileMobilityBlocked(int x, int y);

    bool isTileMobilityBlocked(Tile* t);

    void inspectLineOfSightTiles(int endX, int endY, std::function<void(Tile* t)> inspector);

    void inspectLineOfSightBodies(int endX, int endY, std::function<void(Body* b)> inspector);

    void inspectLineOfSightCharacters(int endX, int endY, std::function<void(Character* c)> inspector, bool onlyLiving = true);

    Character* getClosestVisibleEnemy();


    // Inventory

    bool slotAcceptsItem(EquipmentSlot slot, Item* item);

    void voidEquipmentSlot(EquipmentSlot slot);

    void setEquipmentSlot(EquipmentSlot slot, Item* item);

    bool equipFromInventory(EquipmentSlot slot, unsigned int itemId);

    bool equipFromLoot(EquipmentSlot slot, unsigned int itemId);

    bool equip(EquipmentSlot slot, Item* item);

    void unequip(EquipmentSlot slot);

    void unequipToPosition(EquipmentSlot slot, unsigned int position);

    void swapWeapons();

    bool moveItemToPosition(unsigned int itemId, unsigned int position);

    bool moveEquipmentToPosition(EquipmentSlot slot, unsigned int position);

    bool swapEquipment(EquipmentSlot slotA, EquipmentSlot slotB);

    bool moveLootItemToInventoryPosition(unsigned int itemId, unsigned int position);

    bool moveInventoryItemToLootPosition(unsigned int itemId, unsigned int position);

    bool moveEquipmentToLootPosition(EquipmentSlot slot, unsigned int position);

    bool moveLootItemToLootPosition(unsigned int itemId, unsigned int position);

    void sendInventoryUpdate();
    
    void sendItemUpdateEvent(Item* item);
    
    void sendEquipmentAddedEvent(EquipmentSlot slot);
    
    void sendEquipmentUpdatedEvent(EquipmentSlot slot);
    
    void sendEquipmentRemovedEvent(EquipmentSlot slot);
    
    void sendEquipmentSwappedEvent(EquipmentSlot slotA, EquipmentSlot slotB);

    virtual void onInventoryOpen(Body* guest = NULL);

    virtual void onInventoryClose(Body* guest = NULL);


    // Movement

    bool moveUp();

    bool moveDown();

    bool moveLeft();

    bool moveRight();

    void pathToPosition(float x, float y);

    bool canPathToPosition(float x, float y);

    bool moveToTile(Tile* tile);

    bool canMoveToPosition(Level* targetLevel, float x, float y);

    bool moveToPosition(float x, float y);


    // Mission

    void assignMission(Mission* mission);


    // Management

    void clearLevelSpecificData();

    void sendCombatTurnStart();

    void sendCombatTurnEnd();

    void sendCombatDecisionStart();

    void sendCombatDecisionEnd();

    void setControllingPlayer(Player* player);

    void unsetControllingPlayer();

    void setSpectatingPlayer(Player* player);

    void unsetSpectatingPlayer(Player* player);

    void clearSpectators();

    void getAvailableInteractions(std::vector<Interaction::Type> &fill);

    virtual void handleInteraction(Interaction::Type interaction, Body* interactor, bit::ServerPacket &responsePacket);

    void prepareSnapshot(bit::ServerPacket &packet, bool full = false);


    // Factories

    static std::string getTitle(Character::Type type);

    static std::string getEquipmentSlotTitle(Character::EquipmentSlot slot);

protected:

    // Movement

    bool isTileBlocked(Tile* tile);

    bool isTileBlockedForPathfinding(Tile* tile);


    // Mission

    void checkMissions();
};

#endif