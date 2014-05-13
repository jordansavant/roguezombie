#include "InventoryItemLabel.hpp"
#include "InventoryEquipmentSlot.hpp"
#include "Inventory.hpp"
#include "Hud.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "../ClientRequest.hpp"

InventoryEquipmentSlot::InventoryEquipmentSlot(Hud* hud, Character::EquipmentSlot slot, Inventory* inventory, float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : Frame(hud, relativeX, relativeY, width, height, anchorType, std::bind(&Hud::typicalContainerControl, inventory->hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), slot(slot), inventory(inventory), equippedItemLabel(NULL)
{
    managesOpacity = true;
}

void InventoryEquipmentSlot::addItemLabel(InventoryItemLabel* label)
{
    label->currentEquipmentSlot = this;
    addChild(label);
}

void InventoryEquipmentSlot::removeItemLabel()
{
    if(equippedItemLabel)
    {
        equippedItemLabel->currentEquipmentSlot = NULL;
        equippedItemLabel = NULL;
    }
    clearChildren();
}

unsigned int InventoryEquipmentSlot::addChild(Element* child)
{
    InventoryItemLabel* label = static_cast<InventoryItemLabel*>(child);
    equippedItemLabel = label;
    equippedItemLabel->currentEquipmentSlot = this;
    return bit::Container::addChild(child);
}

void InventoryEquipmentSlot::moveChild(Container* other, unsigned int index)
{
    bit::Container::moveChild(other, index);
    if(equippedItemLabel)
    {
        equippedItemLabel->currentEquipmentSlot = NULL;
        equippedItemLabel = NULL;
    }
}

void InventoryEquipmentSlot::moveChild(Container* other, Element* child)
{
    bit::Container::moveChild(other, child);
}

void InventoryEquipmentSlot::removeChild(unsigned int index)
{
    bit::Container::removeChild(index);
    if(equippedItemLabel)
    {
        equippedItemLabel->currentEquipmentSlot = NULL;
        equippedItemLabel = NULL;
    }
}