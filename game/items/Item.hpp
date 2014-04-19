#pragma once
#ifndef RZ_ITEM_H
#define RZ_ITEM_H

#include "ItemCategory.hpp"
#include <vector>
#include "../../bitengine/Network.hpp"

class Character;

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
    };

    unsigned int CategoryBase;
    unsigned int CategoryArmor;
    unsigned int CategoryWeapon;
    unsigned int CategoryJewelry;
    unsigned int CategoryContainer;

    unsigned int id;
    Type type;
    Item* parentItem;
    Character* parentCharacter;
    float weight;
    bool canContainItems;
    unsigned int itemLimit;
    std::vector<Item*> items;

    bool hasAny(unsigned int currentValue, unsigned int filter);

    bool hasAll(unsigned int currentValue, unsigned int filter);

    void addItem(Item* item);

    Character* getParentCharacter();

    void fillIdHierarchy(std::vector<unsigned int> &fill);

    virtual void packIdHierarchy(bit::ServerPacket &packet);

    void sendAddItemPacket();

    void prepareSnapshot(bit::ServerPacket &packet);

    static std::string getTitle(Type type);

    static Item* create(Type type);
};

#endif