#include "Inventory.hpp"
#include "Hud.hpp"
#include "InventoryItemLabel.hpp"
#include "../../bitengine/Input.hpp"
#include "../ClientRequest.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"

Inventory::Inventory(Hud* _hud)
    : HudMenu(_hud, 0, 0, 1300, 720, bit::Element::AnchorType::Right, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3))
{
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
    equipmentPanel = new bit::Container(5, 0, 490, 710, bit::Element::AnchorType::Left, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    addChild(equipmentPanel);
    
    // 800 x 720
    inventoryPanel = new bit::Container(-5, 0, 790, 710, bit::Element::AnchorType::Right, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    addChild(inventoryPanel);

    // EQUIPMENT BOXES
    int topOffset = 150;
    headBox = new bit::Container(0, topOffset + 0, 200, 100, bit::Element::AnchorType::Top, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    equipmentPanel->addChild(headBox);
    
    chestBox = new bit::Container(0, topOffset + 120, 160, 100, bit::Element::AnchorType::Top, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    equipmentPanel->addChild(chestBox);
    
    legBox = new bit::Container(0, topOffset + 240, 160, 60, bit::Element::AnchorType::Top, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    equipmentPanel->addChild(legBox);
    
    footBox = new bit::Container(0, topOffset + 320, 160, 30, bit::Element::AnchorType::Top, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    equipmentPanel->addChild(footBox);
    
    totemBox = new bit::Container(-130, topOffset + 120, 60, 60, bit::Element::AnchorType::Top, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    equipmentPanel->addChild(totemBox);
    
    handBox = new bit::Container(-130, topOffset + 200, 60, 60, bit::Element::AnchorType::Top, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    equipmentPanel->addChild(handBox);
    
    primaryWeaponBox = new bit::Container(150, topOffset + 120, 100, 100, bit::Element::AnchorType::Top, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    equipmentPanel->addChild(primaryWeaponBox);
    
    secondaryWeaponBox = new bit::Container(150, topOffset + 240, 100, 100, bit::Element::AnchorType::Top, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    equipmentPanel->addChild(secondaryWeaponBox);
}

void Inventory::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    HudMenu::update(window, gameTime);

    equipmentPanel->opacity = opacity;
    inventoryPanel->opacity = opacity;

    headBox->opacity = opacity;
    chestBox->opacity = opacity;
    legBox->opacity = opacity;
    footBox->opacity = opacity;
    totemBox->opacity = opacity;
    handBox->opacity = opacity;
    primaryWeaponBox->opacity = opacity;
    secondaryWeaponBox->opacity = opacity;
}

void Inventory::buildEquipment()
{
    LevelClient* levelClient = hud->state->levelClient;
    if(levelClient->playerCharacter == NULL)
    {
        return;
    }

    // HEAD
    headBox->clearChildren();
    if(levelClient->playerCharacter->has_equipmentSlot_head)
    {
        bit::Label* label = buildItem(&levelClient->playerCharacter->equipmentSlot_head, 0, 0);
        headBox->addChild(label);
    }
}

void Inventory::buildItemList()
{
    LevelClient* levelClient = hud->state->levelClient;
    if(levelClient->playerCharacter == NULL)
    {
        return;
    }

    // clean up
    inventoryPanel->clearChildren();

    float y = 10;
    bit::Label* title = new bit::Label(10, y, 0, 0, bit::Element::AnchorType::TopLeft);
    title->setSfFontSize(24);
    title->setSfFont(hud->journalFont);
    title->normalColor = sf::Color::White;
    title->setSfFontString(std::string("Inventory"));
    inventoryPanel->addChild(title);
    y += 30;

    for(auto iterator = levelClient->playerCharacter->inventoryClient.itemClients.begin(); iterator != levelClient->playerCharacter->inventoryClient.itemClients.end(); iterator++)
    {
        ItemClient* i = &iterator->second;

        InventoryItemLabel* option = buildItem(i, 10, y);
        inventoryPanel->addChild(option);

        y += 30;
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
    immediateEffect(new bit::FadeEffect(300, 1));
}
