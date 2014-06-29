#pragma once
#ifndef RZ_ITEM_H
#define RZ_ITEM_H

#include "ItemCategory.hpp"
#include <vector>
#include "../../bitengine/Network.hpp"

class Body;

class Item
{
public:

    Item();

    virtual ~Item();

    enum Type
    {
        None,
        Backpack,
        HardHat,
        Magnum357,
        Z4Rifle,
        Crowbar,
    };

    struct Schema
    {
        Schema()
            : id(0), CategoryBase(0), CategoryArmor(0), CategoryWeapon(0), CategoryJewelry(0), CategoryContainer(0),
              type(Type::None), weight(0), minimumDamage(0), maximumDamage(0), effectiveRangeInTiles(1), canContainItems(false), itemLimit(0)
        {
        }

        unsigned int id;
        unsigned int CategoryBase;
        unsigned int CategoryArmor;
        unsigned int CategoryWeapon;
        unsigned int CategoryJewelry;
        unsigned int CategoryContainer;
        Type type;
        float weight;
        unsigned int effectiveRangeInTiles;
        unsigned int minimumDamage, maximumDamage;
        bool canContainItems;
        unsigned int itemLimit;

        friend sf::Packet& operator <<(sf::Packet& packet, const Schema &schema)
        {
            packet << sf::Uint32(schema.id);
            packet << sf::Uint32(schema.CategoryBase);
            packet << sf::Uint32(schema.CategoryArmor);
            packet << sf::Uint32(schema.CategoryWeapon);
            packet << sf::Uint32(schema.CategoryJewelry);
            packet << sf::Uint32(schema.CategoryContainer);
            packet << sf::Uint32(schema.type);
            packet << schema.weight;
            packet << sf::Uint32(schema.effectiveRangeInTiles);
            return packet;
        }

        friend sf::Packet& operator >>(sf::Packet& packet, Schema &schema)
        {
            packet >> schema.id;
            packet >> schema.CategoryBase;
            packet >> schema.CategoryArmor;
            packet >> schema.CategoryWeapon;
            packet >> schema.CategoryJewelry;
            packet >> schema.CategoryContainer;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Item::Type>(packet, schema.type);
            packet >> schema.weight;
            packet >> schema.effectiveRangeInTiles;
            return packet;
        }
    };
    Schema schema;
    Item* parentItem;
    Body* parentBody;
    std::vector<Item*> items;

    bool hasAny(unsigned int currentValue, unsigned int filter);

    bool hasAll(unsigned int currentValue, unsigned int filter);

    bool isOfBaseType(ItemCategory::Base);

    bool isOfWeaponType(ItemCategory::Weapon);

    bool isOfArmorType(ItemCategory::Armor);
    
    bool isOfJewelryType(ItemCategory::Jewelry);
    
    bool isOfContainerType(ItemCategory::Container);

    void addItem(Item* item);

    Item* findItem(unsigned int itemId);

    Item* removeItem(unsigned int itemId);

    void onAddToNewParent();

    void onBeforeRemoveFromParent();

    Body* getParentBody();

    void fillIdHierarchy(std::vector<unsigned int> &fill);

    virtual void packIdHierarchy(bit::ServerPacket &packet);

    void prepareSnapshot(bit::ServerPacket &packet);

    void prepareServerEventPacket_itemAdded(bit::ServerPacket &packet);

    void prepareServerEventPacket_itemRemoved(bit::ServerPacket &packet);

    static Item* create(Type type, unsigned int id);

    static std::string getTitle(Type type);

    static std::string getSpriteName(Type type);
};

#endif