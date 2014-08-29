#include "Item.hpp"
#include "ItemCategory.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/Network.hpp"
#include "../ServerEvent.hpp"
#include "../Body.hpp"
#include "../Character.hpp"
#include "../Player.hpp"
#include "../Level.hpp"
#include "../GameplayServer.hpp"

Item::Item()
    : schema(), parentItem(NULL), parentBody(NULL)
{
}

Item::~Item()
{
    for(unsigned int i=0; i < items.size(); i++)
    {
        delete items[i];
    }
}

bool Item::hasAny(unsigned int attribute, unsigned int filter)
{
    return bit::Math::bitwiseHasAny(attribute, filter);
}

bool Item::hasAll(unsigned int attribute, unsigned int filter)
{
    return bit::Math::bitwiseHasAll(attribute, filter);
}

bool Item::isOfBaseType(ItemCategory::Base category)
{
    return hasAny(schema.CategoryBase, category);
}

bool Item::isOfWeaponType(ItemCategory::Weapon category)
{
    return hasAny(schema.CategoryWeapon, category);
}

bool Item::isOfArmorType(ItemCategory::Armor category)
{
    return hasAny(schema.CategoryArmor, category);
}
    
bool Item::isOfJewelryType(ItemCategory::Jewelry category)
{
    return hasAny(schema.CategoryJewelry, category);
}
    
bool Item::isOfContainerType(ItemCategory::Container category)
{
    return hasAny(schema.CategoryContainer, category);
}

void Item::addItem(Item* item)
{
    if(schema.canContainItems && items.size() + 1 <= schema.itemLimit)
    {
        items.push_back(item);
        item->parentItem = this;
        item->onAddToNewParent();
    }
}

Item* Item::findItem(unsigned int itemId)
{
    // Look through my items
    for(auto it = items.begin(); it != items.end();)
    {
        Item* item = (*it);

        // If one of my child items is the match
        if(item->schema.id == itemId)
        {
            // Return the pointer
            return item;
        }
        // If my child is not the match
        else
        {
            // If my child has more children
            Item* childItem = NULL;
            if(item->items.size() > 0)
            {
                // Recurse into the child to see if it has the item
                childItem = item->findItem(itemId);
            }

            // If my recursion found the item
            if(childItem)
            {
                // return it
                return childItem;
            }
            else
            {
                // go to next child item
                ++it;
            }
        }
    }

    // if no child had it, cry
    return NULL;
}

Item* Item::removeItem(unsigned int itemId)
{
    // Look through my items
    for(auto it = items.begin(); it != items.end();)
    {
        Item* item = (*it);

        // If one of my child items is the match
        if(item->schema.id == itemId)
        {
            // Remove it from the list and return the pointer
            item->onBeforeRemoveFromParent();
            items.erase(it);
            item->parentItem = NULL;
            return item;
        }
        // If my child is not the match
        else
        {
            // If my child has more children
            Item* childItem = NULL;
            if(item->items.size() > 0)
            {
                // Recurse into the child to see if it has the item
                childItem = item->removeItem(itemId);
            }

            // If my recursion found the item
            if(childItem)
            {
                // return it
                return childItem;
            }
            else
            {
                // go to next child item
                ++it;
            }
        }
    }

    // if no child had it, cry
    return NULL;
}

unsigned int Item::findAvailablePosition()
{
    // TODO - horrid
    unsigned int position = 0;
    while(true)
    {
        bool vacant = true;
        for(unsigned int i=0; i < items.size(); i++)
        {
            if(position == items[i]->schema.position)
            {
                position++;
                vacant = false;
                break;
            }
        }
        if(vacant)
        {
            return position;
        }
    }
}

Body* Item::getParentBody()
{
    if(parentItem)
    {
        return parentItem->getParentBody();
    }

    return parentBody;
}

// First elements are highest parents
void Item::fillIdHierarchy(std::vector<unsigned int> &fill)
{
    if(parentItem)
    {
        parentItem->fillIdHierarchy(fill);
    }
    fill.push_back(schema.id);
}

void Item::packIdHierarchy(bit::ServerPacket &packet)
{
    std::vector<unsigned int> parents;
    fillIdHierarchy(parents);

    packet << sf::Uint32(parents.size());
    for(unsigned int i=0; i < parents.size(); i++)
    {
        packet << sf::Uint32(parents[i]);
    }
}

void Item::onAddToNewParent()
{
    Body* b = getParentBody();
    if(b->schema.type == Body::Type::Character)
    {
        Character* c = static_cast<Character*>(b);
        if(c->schema.isPlayerCharacter)
        {
            c->level->server->sendEventToClient(*c->schema.player->client, std::bind(&Item::prepareServerEventPacket_itemAdded, this, std::placeholders::_1));
        }
    }
}

void Item::onBeforeRemoveFromParent()
{
    Body* b = getParentBody();
    if(b->schema.type == Body::Type::Character)
    {
        Character* c = static_cast<Character*>(b);
        if(c->schema.isPlayerCharacter)
        {
            c->level->server->sendEventToClient(*c->schema.player->client, std::bind(&Item::prepareServerEventPacket_itemRemoved, this, std::placeholders::_1));
        }
    }
}

void Item::prepareSnapshot(bit::ServerPacket &packet)
{
    packet << schema;

    packet << sf::Uint32(items.size());
    for(unsigned int i=0; i < items.size(); i++)
    {
        packet << sf::Uint32(items[i]->schema.id);
        items[i]->prepareSnapshot(packet);
    }
}

void Item::prepareServerEventPacket_itemAdded(bit::ServerPacket &packet)
{
    packet << sf::Uint32(ServerEvent::ItemAdded);
    packIdHierarchy(packet);
    prepareSnapshot(packet);
}

void Item::prepareServerEventPacket_itemRemoved(bit::ServerPacket &packet)
{
    packet << sf::Uint32(ServerEvent::ItemRemoved);
    packIdHierarchy(packet);
    prepareSnapshot(packet);
}

Item* Item::create(Type type, unsigned int id)
{
    Item* i = 0;

    switch(type)
    {
        case Type::Backpack:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseContainer;
            i->schema.CategoryContainer = ItemCategory::Container::ContainerBackpack;
            i->schema.weight = 0;
            i->schema.canContainItems = true;
            i->schema.itemLimit = 1000;

            break;

        case Type::HardHat:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseArmor;
            i->schema.CategoryArmor = ItemCategory::Armor::ArmorHead;
            i->schema.weight = 1.5;
            i->schema.armorEffectiveness = .3f;

            break;

        case Type::Magnum357:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseWeapon;
            i->schema.CategoryWeapon = ItemCategory::Weapon::WeaponRanged;
            i->schema.weight = 1;
            i->schema.minimumDamage = 6;
            i->schema.maximumDamage = 9;
            i->schema.effectiveRangeInTiles = 5;

            break;

        case Type::Z4Rifle:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseWeapon;
            i->schema.CategoryWeapon = ItemCategory::Weapon::WeaponRanged;
            i->schema.weight = 2;
            i->schema.minimumDamage = 4;
            i->schema.maximumDamage = 15;
            i->schema.effectiveRangeInTiles = 15;

            break;

        case Type::Crowbar:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseWeapon;
            i->schema.CategoryWeapon = ItemCategory::Weapon::WeaponMelee;
            i->schema.weight = 3;
            i->schema.minimumDamage = 4;
            i->schema.maximumDamage = 6;
            i->schema.effectiveRangeInTiles = 1;

            break;
    }

    i->schema.id = id;
    i->schema.type = type;
    return i;
}

std::string Item::getTitle(Type type)
{
    switch(type)
    {
        default:
        case Type::None:
            return "None";

        case Type::Backpack:
            return "Backpack";

        case Type::HardHat:
            return "Hardhat";

        case Type::Magnum357:
            return ".357 Magnum";

        case Type::Z4Rifle:
            return "Z4 Rifle";

        case Type::Crowbar:
            return "Crowbar";
    }
}

std::string Item::getSpriteName(Type type)
{
    switch(type)
    {
        default:
        case Type::None:
            return "";

        case Type::Backpack:
            return "Backpack";

        case Type::HardHat:
            return "HardHat";

        case Type::Magnum357:
            return "Magnum357";

        case Type::Z4Rifle:
            return "Z4Rifle";

        case Type::Crowbar:
            return "Crowbar";
    }
}

std::string Item::getIconName(Type type)
{
    switch(type)
    {
        default:
        case Type::None:
            return "unknown";

        case Type::Magnum357:
            return "Magnum357";
    }
}
