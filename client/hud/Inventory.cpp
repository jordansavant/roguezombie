#include "Inventory.hpp"
#include "Hud.hpp"
#include "InventoryItemLabel.hpp"
#include "InventoryEquipmentSlot.hpp"
#include "InventoryPositionSlot.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/System.hpp"
#include "../../server/ClientRequest.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../../server/Character.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"

Inventory::Inventory(Hud* _hud)
    : HudMenu(_hud, 0, 0, 1300, 720, bit::Element::AnchorType::Right, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), refreshTimer(5), itemCount(0)
{
    managesOpacity = true;
    originX = 0;
    originY = 0;

    /* lool
     __________________________________________
    |   Equiment   |         Inventory         |
    |              |                           |
    |     _O_      |  [ ] --- -----  ---  --   |
    |    / | \     |  [ ] --- -----  ---  --   |
    |     / \      |  [ ] --- -----  ---  --   |
    |    |   |     |  [ ] --- -----  ---  --   |
    |______________|___________________________|

    */

    // PRIMARY PANELS
    // 500 x 720
    equipmentPanel = new Frame(hud, -5, 0, 494, 710, bit::Element::AnchorType::Right, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    equipmentPanel->managesOpacity = true;
    addChild(equipmentPanel);
    
    // 800 x 720
    inventoryPanel = new Frame(hud, 5, 0, 794, 710, bit::Element::AnchorType::Left, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    inventoryPanel->managesOpacity = true;
    addChild(inventoryPanel);


    // INVENTORY SLOTS
    bit::Label* title = new bit::Label(20, 10, 0, 0, bit::Element::AnchorType::TopLeft);
    title->setSfFontSize(24);
    title->setSfFont(hud->journalFont);
    title->normalColor = sf::Color(0, 255, 0);
    title->setSfFontString(std::string("INVENTORY:"));
    inventoryPanel->addChild(title);
    int y = 48;
    int width = 64;
    int pad = 2;
    int x = pad;
    for(unsigned int j=0; j < 120; j++)
    {
        if(x + width + pad > inventoryPanel->targetWidth)
        {
            y += width + pad;
            x = pad;
        }
        InventoryPositionSlot* slot = new InventoryPositionSlot(hud, j, this, x, y, width, width, bit::Element::AnchorType::TopLeft);
        inventoryPanel->addChild(slot);
        positionSlotBoxes.push_back(slot);
        x += width + pad;
    }


    // EQUIPMENT BOXES
    int topOffset = 150;
    equipmentSlotBoxes.resize(Character::EquipmentSlot::_count, NULL);

    equipmentSlotBoxes[Character::EquipmentSlot::Head] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::Head, this, 0, topOffset + 0, 200, 100, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::Head]->AcceptedCategoryArmor = ItemCategory::Armor::ArmorHead;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::Head]);
    
    equipmentSlotBoxes[Character::EquipmentSlot::Chest] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::Chest, this, 0, topOffset + 120, 160, 100, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::Chest]->AcceptedCategoryArmor = ItemCategory::Armor::ArmorChest;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::Chest]);
    
    equipmentSlotBoxes[Character::EquipmentSlot::Legs] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::Legs, this, 0, topOffset + 240, 160, 60, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::Legs]->AcceptedCategoryArmor = ItemCategory::Armor::ArmorLeg;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::Legs]);
    
    equipmentSlotBoxes[Character::EquipmentSlot::Feet] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::Feet, this, 0, topOffset + 320, 160, 30, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::Feet]->AcceptedCategoryArmor = ItemCategory::Armor::ArmorFoot;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::Feet]);
    
    equipmentSlotBoxes[Character::EquipmentSlot::Totem] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::Totem, this, -130, topOffset + 120, 60, 60, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::Totem]->AcceptedCategoryJewelry = ItemCategory::Jewelry::JewelryTotem;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::Totem]);
    
    equipmentSlotBoxes[Character::EquipmentSlot::Hands] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::Hands, this, -130, topOffset + 200, 60, 60, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::Hands]->AcceptedCategoryArmor = ItemCategory::Armor::ArmorHand;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::Hands]);
    
    equipmentSlotBoxes[Character::EquipmentSlot::WeaponPrimary] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::WeaponPrimary, this, 150, topOffset + 120, 100, 100, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::WeaponPrimary]->AcceptedCategoryWeapon = ItemCategory::Weapon::WeaponRanged | ItemCategory::Weapon::WeaponMelee;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::WeaponPrimary]);
    
    equipmentSlotBoxes[Character::EquipmentSlot::WeaponSecondary] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::WeaponSecondary, this, 150, topOffset + 240, 100, 100, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::WeaponSecondary]->AcceptedCategoryWeapon = ItemCategory::Weapon::WeaponRanged | ItemCategory::Weapon::WeaponMelee;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::WeaponSecondary]);
}

void Inventory::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    HudMenu::update(window, gameTime);

    if(refreshTimer.update(gameTime))
    {
        buildEquipment();
        buildItemList(false);
    }
}

void Inventory::buildEquipment()
{
    LevelClient* levelClient = hud->state->levelClient;
    if(levelClient->playerCharacter == NULL)
    {
        return;
    }

    // If the gear is not present, add it
    for(unsigned int i=0; i < Character::EquipmentSlot::_count; i++)
    {
        Character::EquipmentSlot slot = static_cast<Character::EquipmentSlot>(i);

        if(levelClient->playerCharacter->hasEquipment[slot] && equipmentSlotBoxes[slot]->equippedItemLabel == NULL)
        {
            equipmentSlotBoxes[slot]->addItemLabel(buildItem(&levelClient->playerCharacter->equipment[slot], 0, 0));
        }
        // If the gear is present but does not match, remove it
        else if(levelClient->playerCharacter->hasEquipment[slot] && equipmentSlotBoxes[slot]->equippedItemLabel->itemSchema.id != levelClient->playerCharacter->equipment[slot].schema.id)
        {
            equipmentSlotBoxes[slot]->removeItemLabel();
            equipmentSlotBoxes[slot]->addItemLabel(buildItem(&levelClient->playerCharacter->equipment[slot], 0, 0));
        }
        // If the gear is present but not on the player, remove it
        else if(!levelClient->playerCharacter->hasEquipment[slot] && equipmentSlotBoxes[slot]->equippedItemLabel)
        {
            equipmentSlotBoxes[slot]->removeItemLabel();
        }
    }
}

void Inventory::buildItemList(bool force)
{
    // If no player character, skip
    LevelClient* levelClient = hud->state->levelClient;
    if(levelClient->playerCharacter == NULL)
    {
        return;
    }

    // If the number of items matches the last sync, skip // TODO - improve
    if(!force && itemCount == levelClient->playerCharacter->inventoryClient.itemClients.size())
    {
        return;
    }

    // clean up
    itemCount = 0;
    for(unsigned int i=0; i < positionSlotBoxes.size(); i++)
    {
        positionSlotBoxes[i]->clearChildren();
    }

    unsigned int i=0;
    for(auto iterator = levelClient->playerCharacter->inventoryClient.itemClients.begin(); iterator != levelClient->playerCharacter->inventoryClient.itemClients.end(); iterator++)
    {
        ItemClient* item = &iterator->second;
        InventoryItemLabel* option = buildItem(item, 0, 0);
        positionSlotBoxes[i]->addChild(option);
        i++;
        itemCount++;
    }
}

InventoryItemLabel* Inventory::buildItem(ItemClient* item, float x, float y)
{
    return new InventoryItemLabel(this, item, x, y, bit::Element::AnchorType::TopLeft);
}

void Inventory::hide()
{
    canHaveFocus = false;
    isShown = false;
    clearEffects();
    relativePosition.x = 0;
    immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, targetWidth, 0));
    immediateEffect(new bit::FadeEffect(150, 0));
}

void Inventory::show()
{
    buildItemList();
    buildEquipment();

    canHaveFocus = true;
    isShown = true;
    clearEffects();
    relativePosition.x = targetWidth;
    immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, -targetWidth, 0));
    immediateEffect(new bit::FadeEffect(300, Hud::popupOpacity));
}
