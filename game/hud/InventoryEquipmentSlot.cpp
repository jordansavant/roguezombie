#include "InventoryItemLabel.hpp"
#include "InventoryEquipmentSlot.hpp"
#include "Inventory.hpp"
#include "Hud.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "../ClientRequest.hpp"

InventoryEquipmentSlot::InventoryEquipmentSlot(Inventory* inventory, float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : bit::Container(relativeX, relativeY, width, height, anchorType, std::bind(&Hud::typicalContainerControl, inventory->hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), inventory(inventory), equippedItemLabel(NULL)
{
}

void InventoryEquipmentSlot::addItemLabel(InventoryItemLabel* label)
{
    equippedItemLabel = label;
    addChild(label);
}

void InventoryEquipmentSlot::removeItemLabel()
{
    equippedItemLabel = NULL;
    clearChildren();
}

void InventoryEquipmentSlot::moveChild(Container* other, unsigned int index)
{
    bit::Container::moveChild(other, index);
    equippedItemLabel = NULL;
}