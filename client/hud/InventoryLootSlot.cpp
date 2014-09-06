#include "InventoryItemLabel.hpp"
#include "InventoryLootSlot.hpp"
#include "Inventory.hpp"
#include "Hud.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "../../server/ClientRequest.hpp"

InventoryLootSlot::InventoryLootSlot(Hud* hud, unsigned int position, float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : Frame(hud, relativeX, relativeY, width, height, anchorType, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)),
      position(position), equippedItemLabel(NULL)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    managesOpacity = true;
}

void InventoryLootSlot::addItemLabel(InventoryItemLabel* label)
{
    label->currentLootSlot = this;
    addChild(label);
}

void InventoryLootSlot::removeItemLabel()
{
    if(equippedItemLabel)
    {
        equippedItemLabel->currentLootSlot = NULL;
        equippedItemLabel = NULL;
    }
    clearChildren();
}

bool InventoryLootSlot::acceptsLabel(InventoryItemLabel* label)
{
    return true;
}

unsigned int InventoryLootSlot::addChild(Element* child)
{
    InventoryItemLabel* label = static_cast<InventoryItemLabel*>(child);
    equippedItemLabel = label;
    equippedItemLabel->currentLootSlot = this;
    return bit::Container::addChild(child);
}

void InventoryLootSlot::moveChild(Container* other, unsigned int index)
{
    bit::Container::moveChild(other, index);
    if(equippedItemLabel)
    {
        equippedItemLabel->currentLootSlot = NULL;
        equippedItemLabel = NULL;
    }
}

void InventoryLootSlot::moveChild(Container* other, Element* child)
{
    bit::Container::moveChild(other, child);
}

void InventoryLootSlot::removeChild(unsigned int index)
{
    bit::Container::removeChild(index);
    if(equippedItemLabel)
    {
        equippedItemLabel->currentLootSlot = NULL;
        equippedItemLabel = NULL;
    }
}