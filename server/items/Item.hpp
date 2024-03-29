#pragma once
#ifndef RZ_ITEM_H
#define RZ_ITEM_H

#include "ItemCategory.hpp"
#include <vector>
#include "../../bitengine/Network.hpp"
#include "../AccessLevel.hpp"

class Body;
class Character;
class Tile;
class Level;

class Item
{
public:

    Item();

    virtual ~Item();

    enum Type
    {
        None, // 0
        Backpack, // 1
        HardHat, // 2
        Magnum357, // 3
        Z4Rifle, // 4
        Crowbar, // 5
        Baton, // 6
        Medkit, // 7
        Brick, // 8
        Grenade, // 9
        FootballPads, // 10
        CombatBoots, // 11
        RacingPants, // 12
        CleaningGloves, // 13
        GoldMedal, // 14
        KeyCard, // 15
        Biosprayer, // 16
        Wand, // 17
    };

    enum CommandType
    {
        CommandTypeNone,
        CommandTypeSelf,
        CommandTypeCharacter,
        CommandTypeStructure,
        CommandTypeArea,
    };

    struct Schema
    {
        Schema()
            : id(0), position(0), CategoryBase(0), CategoryArmor(0), CategoryWeapon(0), CategoryJewelry(0), CategoryContainer(0),
              type(Type::None), weight(0), minimumDamage(0), maximumDamage(0), effectiveRangeInTiles(1), effectiveRadiusInTiles(1), maximumRangeInTiles(999), armorEffectiveness(0),
              canContainItems(false), itemLimit(0), commandType(CommandType::CommandTypeNone), canCommandInCombat(false), canCommandInFree(false), accessLevel(AccessLevel::None)
        {
        }

        unsigned int id;
        unsigned int position;
        unsigned int CategoryBase;
        unsigned int CategoryArmor;
        unsigned int CategoryWeapon;
        unsigned int CategoryJewelry;
        unsigned int CategoryContainer;
        Type type;
        float weight;
        unsigned int effectiveRangeInTiles;
        unsigned int effectiveRadiusInTiles;
        unsigned int maximumRangeInTiles;
        unsigned int minimumDamage, maximumDamage;
        float armorEffectiveness;
        bool canContainItems;
        unsigned int itemLimit;
        CommandType commandType;
        bool canCommandInCombat;
        bool canCommandInFree;
        AccessLevel accessLevel;

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            packet << sf::Uint32(schema.id);
            packet << sf::Uint32(schema.position);
            packet << sf::Uint32(schema.CategoryBase);
            packet << sf::Uint32(schema.CategoryArmor);
            packet << sf::Uint32(schema.CategoryWeapon);
            packet << sf::Uint32(schema.CategoryJewelry);
            packet << sf::Uint32(schema.CategoryContainer);
            packet << sf::Uint32(schema.type);
            packet << schema.weight;
            packet << sf::Uint32(schema.effectiveRangeInTiles);
            packet << sf::Uint32(schema.effectiveRadiusInTiles);
            packet << sf::Uint32(schema.maximumRangeInTiles);
            packet << sf::Uint32(schema.minimumDamage);
            packet << sf::Uint32(schema.maximumDamage);
            packet << schema.armorEffectiveness;
            packet << sf::Uint32(schema.commandType);
            packet << schema.canCommandInCombat;
            packet << schema.canCommandInFree;
            return packet;
        }

        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            packet >> schema.id;
            packet >> schema.position;
            packet >> schema.CategoryBase;
            packet >> schema.CategoryArmor;
            packet >> schema.CategoryWeapon;
            packet >> schema.CategoryJewelry;
            packet >> schema.CategoryContainer;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Item::Type>(packet, schema.type);
            packet >> schema.weight;
            packet >> schema.effectiveRangeInTiles;
            packet >> schema.effectiveRadiusInTiles;
            packet >> schema.maximumRangeInTiles;
            packet >> schema.minimumDamage;
            packet >> schema.maximumDamage;
            packet >> schema.armorEffectiveness;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Item::CommandType>(packet, schema.commandType);
            packet >> schema.canCommandInCombat;
            packet >> schema.canCommandInFree;
            return packet;
        }

        bool hasAny(unsigned int currentValue, unsigned int filter);

        bool hasAll(unsigned int currentValue, unsigned int filter);

        bool isOfBaseType(ItemCategory::Base);

        bool isOfWeaponType(ItemCategory::Weapon);

        bool isOfArmorType(ItemCategory::Armor);
    
        bool isOfJewelryType(ItemCategory::Jewelry);
    
        bool isOfContainerType(ItemCategory::Container);
    };
    Schema schema;
    Item* parentItem;
    Body* parentBody;
    std::vector<Item*> items;
    std::function<bool(Character*)> applyToCharacter;
    std::function<bool(Tile*)> applyToArea;
    std::function<void(Character*)> onUse;
    std::function<void(Character* self, Character* other)> onAttack;

    void addItem(Item* item);

    Item* findItem(unsigned int itemId);

    Item* findItemBy(std::function<bool(Item*)> inspector);

    Item* findItemByType(Type type);

	Item* findItemByTypeAndAccessLevel(Type type, AccessLevel accessLevel);

    Item* removeItem(unsigned int itemId);

    Item* removeItemBy(std::function<bool(Item*)> inspector);

    unsigned int findAvailablePosition();

    void onAddToNewParent();

    void onBeforeRemoveFromParent();

    Body* getParentBody();

    void fillIdHierarchy(std::vector<unsigned int> &fill);

    virtual void packIdHierarchy(bit::ServerPacket &packet);

    void prepareSnapshot(bit::ServerPacket &packet);

    void prepareServerEventPacket_itemAdded(bit::ServerPacket &packet);

    void prepareServerEventPacket_itemRemoved(bit::ServerPacket &packet);

    static bool isEquippable(Item::Schema &schema);

    static void useItemOnSelf(Character* self, Item::Schema &itemSchema);

    static void useItemOnCharacter(Character* self, Character* other, Item::Schema &itemSchema);

    static void useItemOnTileArea(Character* self, Tile* tile, Item::Schema &itemSchema);

    static Item* create(Type type, unsigned int id);

	static Item* create(Type type, AccessLevel accessLevel, unsigned int id);

    static std::string getTitle(Type type);

    static std::string getDescription(Type type);

    static std::string getSpriteName(Type type);

    static std::string getIconName(Type type);

    static void visualizeWeaponFire(Character* user);

    static void visualizeMagicFire(Character* user);

    static void visualizePoisonFire(Character* user);

};

#endif