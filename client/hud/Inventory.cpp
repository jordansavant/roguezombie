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
#include <string>
#include <iomanip>

Inventory::Inventory(Hud* _hud)
    : HudMenu(_hud, 0, 0, 691, 728, bit::Element::AnchorType::Right, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), pendingInventoryRequests(0), refreshTimer(5)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
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
    equipmentPanel = new Frame(hud, 0, 0, 400, targetHeight, bit::Element::AnchorType::TopRight, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    equipmentPanel->scaleStyle = ScaleStyle::PowerOfTwo;
    equipmentPanel->managesOpacity = true;
    addChild(equipmentPanel);
    
    // 800 x 720
    inventoryPanel = new Frame(hud, 0, 0, 296, targetHeight, bit::Element::AnchorType::Left, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    inventoryPanel->scaleStyle = ScaleStyle::PowerOfTwo;
    inventoryPanel->managesOpacity = true;
    addChild(inventoryPanel);

    // Icon
    naviconSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("InventoryNavicon");
    naviconQuadIndex = hud->interfaceVertexMap.requestVertexIndex();

    // Equipment silohuette
    siloSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("Silohuette");
    siloQuadIndex = hud->interfaceVertexMap.requestVertexIndex();

    // Item Data Panel
    int itemDataHeight = 250;
    itemDataPanel = new bit::Container(0, targetHeight - itemDataHeight, 400, itemDataHeight, bit::Element::AnchorType::TopLeft, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    itemDataPanel->scaleStyle = ScaleStyle::PowerOfTwo;
    itemDataPanel->managesOpacity = true;
    equipmentPanel->addChild(itemDataPanel);

    itemDataText = new bit::Label(32, 16, 0, 0, AnchorType::TopLeft);
    itemDataText->setSfFontSize(Hud::font_primarySize);
    itemDataText->setSfFont(hud->journalFont);
    itemDataText->setSfFontString(std::string(""));
    itemDataText->normalColor = Hud::font_primaryColor;
    itemDataText->scaleStyle = ScaleStyle::PowerOfTwo;
    itemDataPanel->addChild(itemDataText);

    // INVENTORY SLOTS
    int y = 8;
    int width = 64;
    int pad = 8;
    int x = pad;
    for(unsigned int j=0; j < 40; j++)
    {
        if(x + width + pad > inventoryPanel->targetWidth)
        {
            y += width + pad;
            x = pad;
        }
        InventoryPositionSlot* slot = new InventoryPositionSlot(hud, j, x, y, width, width, bit::Element::AnchorType::TopLeft);
        inventoryPanel->addChild(slot);
        positionSlotBoxes.push_back(slot);
        x += width + pad;
    }


    // EQUIPMENT BOXES
    int topOffset = 121;
    int leftOffset = 40;
    int size = 64;
    equipmentSlotBoxes.resize(Character::EquipmentSlot::_count, NULL);

    equipmentSlotBoxes[Character::EquipmentSlot::Head] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::Head, 0 + leftOffset, topOffset + 0, size, size, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::Head]->AcceptedCategoryArmor = ItemCategory::Armor::ArmorHead;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::Head]);
    
    equipmentSlotBoxes[Character::EquipmentSlot::Chest] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::Chest, 0 + leftOffset, topOffset + 28*1 + size*1, size, size, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::Chest]->AcceptedCategoryArmor = ItemCategory::Armor::ArmorChest;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::Chest]);
    
    equipmentSlotBoxes[Character::EquipmentSlot::Legs] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::Legs, 0 + leftOffset, topOffset + 28*2 + size*2, size, size, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::Legs]->AcceptedCategoryArmor = ItemCategory::Armor::ArmorLeg;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::Legs]);
    
    equipmentSlotBoxes[Character::EquipmentSlot::Feet] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::Feet, 0 + leftOffset, topOffset + 28*3 + size*3, size, size, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::Feet]->AcceptedCategoryArmor = ItemCategory::Armor::ArmorFoot;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::Feet]);
    
    equipmentSlotBoxes[Character::EquipmentSlot::Totem] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::Totem, leftOffset + 62 + 28*1, topOffset + 28*1 + size*1 - 20, size, size, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::Totem]->AcceptedCategoryJewelry = ItemCategory::Jewelry::JewelryTotem;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::Totem]);
    
    equipmentSlotBoxes[Character::EquipmentSlot::Hands] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::Hands, leftOffset - 62 - 28*1, topOffset + 28*1 + size*1 + 20, size, size, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::Hands]->AcceptedCategoryArmor = ItemCategory::Armor::ArmorHand;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::Hands]);
    
    equipmentSlotBoxes[Character::EquipmentSlot::WeaponPrimary] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::WeaponPrimary, leftOffset - 176, topOffset + 208, size, size, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::WeaponPrimary]->AcceptedCategoryWeapon = ItemCategory::Weapon::WeaponRanged | ItemCategory::Weapon::WeaponMelee;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::WeaponPrimary]);
    
    equipmentSlotBoxes[Character::EquipmentSlot::WeaponSecondary] = new InventoryEquipmentSlot(hud, Character::EquipmentSlot::WeaponSecondary, leftOffset - 172 + size*1, topOffset + 212 + size*1, size, size, bit::Element::AnchorType::Top);
    equipmentSlotBoxes[Character::EquipmentSlot::WeaponSecondary]->AcceptedCategoryWeapon = ItemCategory::Weapon::WeaponRanged | ItemCategory::Weapon::WeaponMelee;
    equipmentPanel->addChild(equipmentSlotBoxes[Character::EquipmentSlot::WeaponSecondary]);
}

void Inventory::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    HudMenu::update(window, gameTime);

    if(pendingInventoryRequests == 0 && refreshTimer.update(gameTime))
    {
        buildEquipment();
        buildItemList(false);
    }
}


void Inventory::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    HudMenu::updateTargets(window, gameTime);

    // Navicon
    bit::Vertex3* naviconQuad = &hud->interfaceVertexMap.vertexArray[naviconQuadIndex];
    bit::VertexHelper::colorQuad(naviconQuad, sf::Color(255, 255, 255, opacity * 255));

    // Silohuette
    bit::Vertex3* siloQuad = &hud->interfaceVertexMap.vertexArray[siloQuadIndex];
    bit::VertexHelper::colorQuad(siloQuad, sf::Color(255, 255, 255, opacity * 255));
}

void Inventory::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    HudMenu::updateReals(window, gameTime);

    // Navicon
    float naviconZ = Hud::getDrawDepth(Hud::zindex_frames);
    bit::Vertex3* naviconQuad = &hud->interfaceVertexMap.vertexArray[naviconQuadIndex];
    naviconSprite->applyToQuad(naviconQuad);
    bit::VertexHelper::positionQuad(naviconQuad, left, top - naviconSprite->height * elementScale, naviconZ, naviconSprite->width, naviconSprite->height, elementScale);

    // Silohuette for Equipment
    int xOff = 40 * elementScale;
    int yOff = 72 * elementScale;
    float siloZ = Hud::getDrawDepth(Hud::zindex_frames);
    bit::Vertex3* siloQuad = &hud->interfaceVertexMap.vertexArray[siloQuadIndex];
    siloSprite->applyToQuad(siloQuad);
    bit::VertexHelper::positionQuad(siloQuad, equipmentPanel->left + xOff, equipmentPanel->top + yOff, siloZ, siloSprite->width, siloSprite->height, elementScale * 2);
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

        // If we have a slot that does not have an item icon for the item on the character
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

    // Check if it matches before jacking with the GUI
    if(!force)
    {
        bool match = true;

        // Check that all items on the player are correct in the GUI
        for(auto iterator = levelClient->playerCharacter->inventoryClient.itemClients.begin(); iterator != levelClient->playerCharacter->inventoryClient.itemClients.end(); iterator++)
        {
            ItemClient* item = &iterator->second;
            unsigned int position = item->schema.position;
            unsigned int itemId = item->schema.id;
            if(position < positionSlotBoxes.size())
            {
                if(positionSlotBoxes[position]->equippedItemLabel == NULL || positionSlotBoxes[position]->equippedItemLabel->itemSchema.id != itemId)
                {
                    match = false;
                    break;
                }
            }
        }

        // Check that all items in the GUI match the items in the player
        if(match)
        {
            for(unsigned int i=0; i < positionSlotBoxes.size(); i++)
            {
                Item::Schema* labelItem = positionSlotBoxes[i]->equippedItemLabel ? &positionSlotBoxes[i]->equippedItemLabel->itemSchema : NULL;
                Item::Schema* playerItem = NULL;

                if(labelItem)
                {
                    auto result = levelClient->playerCharacter->inventoryClient.itemClients.find(labelItem->id);
                    if(result != levelClient->playerCharacter->inventoryClient.itemClients.end())
                    {
                        playerItem = &result->second.schema;
                    }
                }

                if(labelItem)
                {
                    if(playerItem == NULL || labelItem->position != playerItem->position)
                    {
                        match = false;
                        break;
                    }
                }
            }
        }

        if(match)
            return;
    }

    // clean up
    for(unsigned int i=0; i < positionSlotBoxes.size(); i++)
    {
        positionSlotBoxes[i]->clearChildren();
    }
    // rebuild
    for(auto iterator = levelClient->playerCharacter->inventoryClient.itemClients.begin(); iterator != levelClient->playerCharacter->inventoryClient.itemClients.end(); iterator++)
    {
        ItemClient* item = &iterator->second;
        InventoryItemLabel* option = buildItem(item, 0, 0);
        positionSlotBoxes[item->schema.position]->addChild(option);
    }
}

InventoryItemLabel* Inventory::buildItem(ItemClient* item, float x, float y)
{
    return new InventoryItemLabel(hud, item, x, y, bit::Element::AnchorType::TopLeft);
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

void Inventory::showItemData(Item::Schema& itemSchema)
{
    std::stringstream ss;
    ss  << Item::getTitle(itemSchema.type) << "\n"
        << Item::getDescription(itemSchema.type) << "\n\n"
        << "Range..............." << itemSchema.effectiveRangeInTiles << "\n"
        << "Damage.............." << itemSchema.minimumDamage << "-" << itemSchema.maximumDamage << "\n"
        << "Armor..............." << std::setprecision(4) << itemSchema.armorEffectiveness;
    itemDataText->setSfFontString(ss.str());
}

void Inventory::hideItemData()
{
    itemDataText->setSfFontString(std::string());
}