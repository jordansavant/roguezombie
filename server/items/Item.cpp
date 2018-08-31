#include "Item.hpp"
#include "ItemCategory.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/Network.hpp"
#include "../ServerEvent.hpp"
#include "../Body.hpp"
#include "../Character.hpp"
#include "../Tile.hpp"
#include "../Player.hpp"
#include "../Level.hpp"
#include "../lights/Flare.hpp"
#include "../GameplayServer.hpp"

Item::Item()
    : schema(), parentItem(NULL), parentBody(NULL), applyToCharacter(NULL), applyToArea(NULL), onUse(NULL)
{
}

Item::~Item()
{
    for(unsigned int i=0; i < items.size(); i++)
    {
        delete items[i];
    }
}

bool Item::Schema::hasAny(unsigned int attribute, unsigned int filter)
{
    return bit::Math::bitwiseHasAny(attribute, filter);
}

bool Item::Schema::hasAll(unsigned int attribute, unsigned int filter)
{
    return bit::Math::bitwiseHasAll(attribute, filter);
}

bool Item::Schema::isOfBaseType(ItemCategory::Base category)
{
    return hasAny(CategoryBase, category);
}

bool Item::Schema::isOfWeaponType(ItemCategory::Weapon category)
{
    return hasAny(CategoryWeapon, category);
}

bool Item::Schema::isOfArmorType(ItemCategory::Armor category)
{
    return hasAny(CategoryArmor, category);
}
    
bool Item::Schema::isOfJewelryType(ItemCategory::Jewelry category)
{
    return hasAny(CategoryJewelry, category);
}
    
bool Item::Schema::isOfContainerType(ItemCategory::Container category)
{
    return hasAny(CategoryContainer, category);
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
    return findItemBy([itemId] (Item* item) -> bool {
        return item->schema.id == itemId;
    });
}

Item* Item::findItemBy(std::function<bool(Item*)> inspector)
{
    // Look through my items
    for(auto it = items.begin(); it != items.end();)
    {
        Item* item = (*it);

        // If one of my child items is the match
        if(inspector(item))
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
                childItem = item->findItemBy(inspector);
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

Item* Item::findItemByType(Type type)
{
    return findItemBy([type](Item* item) -> bool {
        return item->schema.type == type;
    });
}

Item* Item::findItemByTypeAndAccessLevel(Type type, AccessLevel accessLevel)
{
	// Used to determine if access can be granted to various restricted objects
	return findItemBy([type, accessLevel](Item* item) -> bool {
		return item->schema.type == type && item->schema.accessLevel == accessLevel;
	});
}

Item* Item::removeItem(unsigned int itemId)
{
    return removeItemBy([itemId] (Item* item) -> bool {
        return item->schema.id == itemId;
    });
}

Item* Item::removeItemBy(std::function<bool(Item*)> inspector)
{
    // Look through my items
    for(auto it = items.begin(); it != items.end();)
    {
        Item* item = (*it);

        // If one of my child items is the match
        if(inspector(item))
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
                childItem = item->removeItemBy(inspector);
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

bool Item::isEquippable(Item::Schema &schema)
{
    return
        bit::Math::bitwiseHasAny(schema.CategoryBase, ItemCategory::Base::BaseArmor) ||
        bit::Math::bitwiseHasAny(schema.CategoryBase, ItemCategory::Base::BaseWeapon) ||
        bit::Math::bitwiseHasAny(schema.CategoryBase, ItemCategory::Base::BaseJewelry);
}

void Item::useItemOnSelf(Character* self, Item::Schema &itemSchema)
{
    // Find the item in the character inventory
    Item* item = self->inventory->removeItemBy([&itemSchema] (Item* item) -> bool {
        return item->schema.type == itemSchema.type;
    });
    
    // Pull it out and apply it
    if(item && item->applyToCharacter && item->applyToCharacter(self))
    {
        delete item;
    }
}

void Item::useItemOnCharacter(Character* self, Character* other, Item::Schema &itemSchema)
{
    // Find the item in the character inventory
    Item* item = self->inventory->removeItemBy([&itemSchema] (Item* item) -> bool {
        return item->schema.type == itemSchema.type;
    });
    
    // Pull it out and apply it
    if(item && item->applyToCharacter && item->applyToCharacter(other))
    {
        delete item;
    }
}

void Item::useItemOnTileArea(Character* self, Tile* tile, Item::Schema &itemSchema)
{
    // Find the item in the character inventory
    Item* item = self->inventory->removeItemBy([&itemSchema] (Item* item) -> bool {
        return item->schema.type == itemSchema.type;
    });
    
    // Pull it out and apply it
    if(item && item->applyToArea && item->applyToArea(tile))
    {
        delete item;
    }
}

Item* Item::create(Type type, unsigned int id)
{
	return create(type, AccessLevel::None, id);
}

Item* Item::create(Type type, AccessLevel accessLevel, unsigned int id)
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
            i->onUse = [i] (Character* user) -> void
            {
                // Visualize
                i->visualizeWeaponFire(user);

                // Event
                Item* ix = i;
                user->level->sendEventToAllPlayers([ix, user] (bit::ServerPacket &packet) {
                    packet << sf::Uint32(ServerEvent::Gunfire);
                    packet << user->Body::schema.x;
                    packet << user->Body::schema.y;
                    packet << sf::Uint32(ix->schema.type);
                });
            };

            break;

        case Type::Z4Rifle:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseWeapon;
            i->schema.CategoryWeapon = ItemCategory::Weapon::WeaponRanged;
            i->schema.weight = 2;
            i->schema.minimumDamage = 4;
            i->schema.maximumDamage = 15;
            i->schema.effectiveRangeInTiles = 15;
            i->onUse = [i] (Character* user) -> void
            {
                // Visualize
                i->visualizeWeaponFire(user);

                // Event
                Item* ix = i;
                user->level->sendEventToAllPlayers([ix, user] (bit::ServerPacket &packet) {
                    packet << sf::Uint32(ServerEvent::Gunfire);
                    packet << user->Body::schema.x;
                    packet << user->Body::schema.y;
                    packet << sf::Uint32(ix->schema.type);
                });
            };

            break;

        case Type::Crowbar:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseWeapon;
            i->schema.CategoryWeapon = ItemCategory::Weapon::WeaponMelee;
            i->schema.weight = 3;
            i->schema.minimumDamage = 4;
            i->schema.maximumDamage = 6;
            i->schema.effectiveRangeInTiles = 1;
            i->onUse = [i] (Character* user) -> void
            {
                // Event
                user->level->sendEventToAllPlayers([user] (bit::ServerPacket &packet) {
                    packet << sf::Uint32(ServerEvent::BluntHit);
                    packet << user->Body::schema.x;
                    packet << user->Body::schema.y;
                });
            };

            break;

        case Type::Baton:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseWeapon;
            i->schema.CategoryWeapon = ItemCategory::Weapon::WeaponMelee;
            i->schema.weight = 3;
            i->schema.minimumDamage = 3;
            i->schema.maximumDamage = 5;
            i->schema.effectiveRangeInTiles = 1;
            i->onUse = [i](Character* user) -> void
            {
                // Event
                user->level->sendEventToAllPlayers([user](bit::ServerPacket &packet) {
                    packet << sf::Uint32(ServerEvent::BluntHit);
                    packet << user->Body::schema.x;
                    packet << user->Body::schema.y;
                });
            };

            break;

        case Type::Medkit:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseNone;
            i->schema.CategoryWeapon = ItemCategory::Weapon::WeaponNone;
            i->schema.weight = 1;
            i->schema.effectiveRangeInTiles = 0;
            i->schema.minimumDamage = 10;
            i->schema.maximumDamage = 20;
            i->schema.commandType = CommandType::CommandTypeSelf;
            i->schema.canCommandInCombat = true;
            i->schema.canCommandInFree = true;
            i->applyToCharacter = [i] (Character* self) -> bool
            {
                self->heal(bit::Math::random(i->schema.minimumDamage, i->schema.maximumDamage));

                // Event
                self->level->sendEventToAllPlayers([self] (bit::ServerPacket &packet) {
                    packet << sf::Uint32(ServerEvent::CharacterHeal);
                    packet << self->Body::schema.x;
                    packet << self->Body::schema.y;
                });

                return true;
            };

            break;

        case Type::Brick:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseNone;
            i->schema.CategoryWeapon = ItemCategory::Weapon::WeaponNone;
            i->schema.weight = 5;
            i->schema.effectiveRangeInTiles = 5;
            i->schema.minimumDamage = 5;
            i->schema.maximumDamage = 10;
            i->schema.commandType = CommandType::CommandTypeCharacter;
            i->schema.canCommandInCombat = true;
            i->schema.canCommandInFree = false;
            i->applyToCharacter = [i] (Character* other) -> bool
            {
                other->harm(bit::Math::random(i->schema.minimumDamage, i->schema.maximumDamage));

                // Event
                float x = other->Body::schema.x;
                float y = other->Body::schema.y;
                other->level->sendEventToAllPlayers([x, y] (bit::ServerPacket &packet) {
                    packet << sf::Uint32(ServerEvent::BluntHit);
                    packet << x;
                    packet << y;
                });

                return true;
            };

            break;

        case Type::Grenade:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseNone;
            i->schema.CategoryWeapon = ItemCategory::Weapon::WeaponNone;
            i->schema.weight = 1;
            i->schema.effectiveRangeInTiles = 7;
            i->schema.effectiveRadiusInTiles = 7;
            i->schema.minimumDamage = 20;
            i->schema.maximumDamage = 30;
            i->schema.commandType = CommandType::CommandTypeArea;
            i->schema.canCommandInCombat = true;
            i->schema.canCommandInFree = false;
            i->applyToArea = [i] (Tile* tile) -> bool
            {
                // Effect tiles
                Item* ix = i;
                tile->level->iterateTiles([ix, tile] (unsigned int index, unsigned int x, unsigned int y, Tile* inspectedTile)
                {
                    // If the tile is in the radius
                    if(bit::VectorMath::distance(tile->schema.x, tile->schema.y, inspectedTile->schema.x, inspectedTile->schema.y) < 4 * tile->level->tileWidth)
                    {
                        // If there is a character on it, hurt it
                        if(inspectedTile->body && inspectedTile->body->schema.type == Body::Type::Character)
                        {
                            Character* other = static_cast<Character*>(inspectedTile->body);
                            other->harm(bit::Math::random(ix->schema.minimumDamage, ix->schema.maximumDamage));
                        }
                    }
                });

                // Visual effects
                sf::Color s = sf::Color(255, 155, 0);
                sf::Color e = sf::Color::Black;
                tile->level->createLightFlare(tile->schema.x, tile->schema.y, 2, s, e, ix->schema.effectiveRadiusInTiles, ix->schema.effectiveRadiusInTiles, 1, 0);

                // Event
                tile->level->sendEventToAllPlayers([tile] (bit::ServerPacket &packet) {
                    packet << sf::Uint32(ServerEvent::Explosion);
                    packet << tile->schema.x;
                    packet << tile->schema.y;
                });

                return true;
            };

            break;

        case Type::FootballPads:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseArmor;
            i->schema.CategoryArmor = ItemCategory::Armor::ArmorChest;
            i->schema.weight = 5;
            i->schema.armorEffectiveness = .4f;

            break;

        case Type::CombatBoots:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseArmor;
            i->schema.CategoryArmor = ItemCategory::Armor::ArmorFoot;
            i->schema.weight = 2;
            i->schema.armorEffectiveness = .5f;

            break;

        case Type::RacingPants:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseArmor;
            i->schema.CategoryArmor = ItemCategory::Armor::ArmorLeg;
            i->schema.weight = 2;
            i->schema.armorEffectiveness = .3f;

            break;

        case Type::CleaningGloves:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseArmor;
            i->schema.CategoryArmor = ItemCategory::Armor::ArmorHand;
            i->schema.weight = .2;
            i->schema.armorEffectiveness = .1f;

            break;

        case Type::GoldMedal:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseJewelry;
            i->schema.CategoryJewelry = ItemCategory::Jewelry::JewelryTotem;
            i->schema.weight = .01;
            i->schema.armorEffectiveness = 0;

            break;

        case Type::KeyCard:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseNone;
            i->schema.CategoryWeapon = ItemCategory::Weapon::WeaponNone;
            i->schema.weight = .01;

            break;

        case Type::Biosprayer:

            i = new Item();
            i->schema.CategoryBase = ItemCategory::Base::BaseWeapon;
            i->schema.CategoryWeapon = ItemCategory::Weapon::WeaponRanged;
            i->schema.weight = 4;
            i->schema.minimumDamage = 4;
            i->schema.maximumDamage = 8;
            i->schema.effectiveRangeInTiles = 2;
            i->schema.maximumRangeInTiles = 2;
            i->onUse = [i](Character* user) -> void
            {
                // Visualize
                i->visualizeWeaponFire(user);

                // Event
                Item* ix = i;
                user->level->sendEventToAllPlayers([ix, user](bit::ServerPacket &packet) {
                    packet << sf::Uint32(ServerEvent::Gunfire);
                    packet << user->Body::schema.x;
                    packet << user->Body::schema.y;
                    packet << sf::Uint32(ix->schema.type);
                });
            };

            break;
    }

    i->schema.id = id;
    i->schema.type = type;
	i->schema.accessLevel = accessLevel;
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

        case Type::Baton:
            return "Baton";

        case Type::Medkit:
            return "Medkit";

        case Type::Brick:
            return "Brick";

        case Type::Grenade:
            return "Grenade";

        case Type::FootballPads:
            return "Football Pads";

        case Type::CombatBoots:
            return "Combat Boots";

        case Type::RacingPants:
            return "Racing Pants";

        case Type::CleaningGloves:
            return "Cleaning Gloves";

        case Type::GoldMedal:
            return "Gold Medal";

        case Type::KeyCard:
            return "Key Card";

        case Type::Biosprayer:
            return "Biosprayer";
    }
}

// One line, max 23 chars
std::string Item::getDescription(Type type)
{
    switch(type)
    {
        default:
        case Type::None:
            return "No description";

        case Type::Backpack:
            return "It's a backpack";

        case Type::HardHat:
            return "OSHA mandated protection";

        case Type::Magnum357:
            return "Are you feelin' lucky?";

        case Type::Z4Rifle:
            return "Future modern war rifle";

        case Type::Crowbar:
            return "100% Freeman made";

        case Type::Baton:
            return "Pick up that can!";

        case Type::Medkit:
            return "Instantly recover health";

        case Type::Brick:
            return "Throw at people";

        case Type::Grenade:
            return "Explodes in a radius";

        case Type::FootballPads:
            return "American that is...";

        case Type::CombatBoots:
            return "Function over form";

        case Type::RacingPants:
            return "Protection and badassery";

        case Type::CleaningGloves:
            return "Keep it clean";

        case Type::GoldMedal:
            return "Congrats you're #1";

        case Type::KeyCard:
            return "Opens chests and doors";

        case Type::Biosprayer:
            return "Toxifies enemies";
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

        case Type::Baton:
            return "Baton";

        case Type::FootballPads:
            return "FootballPads";

        case Type::CombatBoots:
            return "CombatBoots";

        case Type::RacingPants:
            return "RacingPants";

        case Type::CleaningGloves:
            return "CleaningGloves";

        case Type::KeyCard:
            return "KeyCard";

        case Type::Biosprayer:
            return "Biosprayer";
    }
}

std::string Item::getIconName(Type type)
{
    switch(type)
    {
        default:
        case Type::None:
            return "unknown";

        case Type::HardHat:
            return "HardHat";

        case Type::Magnum357:
            return "Magnum357";

        case Type::Z4Rifle:
            return "Z4Rifle";

        case Type::Crowbar:
            return "Crowbar";

        case Type::Baton:
            return "Baton";

        case Type::Medkit:
            return "Medkit";

        case Type::Brick:
            return "Brick";

        case Type::Grenade:
            return "Grenade";

        case Type::FootballPads:
            return "FootballPads";

        case Type::CombatBoots:
            return "CombatBoots";

        case Type::RacingPants:
            return "RacingPants";

        case Type::CleaningGloves:
            return "CleaningGloves";

        case Type::GoldMedal:
            return "GoldMedal";

        case Type::KeyCard:
            return "KeyCard";

        case Type::Biosprayer:
            return "Biosprayer";
    }
}

void Item::visualizeWeaponFire(Character* user)
{
    // Firing flare
    sf::Color s = sf::Color::Yellow;;
    sf::Color e = sf::Color::Black;
    user->level->createLightFlare(user->Body::schema.x, user->Body::schema.y, 0.5f, s, e, 2, 2, 1, 0);
}

