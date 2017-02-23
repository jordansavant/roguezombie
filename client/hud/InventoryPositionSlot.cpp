#include "InventoryItemLabel.hpp"
#include "InventoryPositionSlot.hpp"
#include "Inventory.hpp"
#include "Hud.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "../../server/ClientRequest.hpp"

InventoryPositionSlot::InventoryPositionSlot(Hud* hud, unsigned int position, float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : Frame(hud, relativeX, relativeY, width, height, anchorType, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)),
      position(position), equippedItemLabel(NULL)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    managesOpacity = true;
}

void InventoryPositionSlot::addItemLabel(InventoryItemLabel* label)
{
    label->currentPositionSlot = this;
    addChild(label);
}

void InventoryPositionSlot::removeItemLabel()
{
    if(equippedItemLabel)
    {
        equippedItemLabel->currentPositionSlot = NULL;
        equippedItemLabel = NULL;
    }
    clearChildren();
}

bool InventoryPositionSlot::acceptsLabel(InventoryItemLabel* label)
{
    return true;
}

unsigned int InventoryPositionSlot::addChild(Element* child)
{
    InventoryItemLabel* label = static_cast<InventoryItemLabel*>(child);
    equippedItemLabel = label;
    equippedItemLabel->currentPositionSlot = this;
    label->itemSchema.position = position;
    return bit::Container::addChild(child);
}

void InventoryPositionSlot::moveChild(Container* other, unsigned int index)
{
    bit::Container::moveChild(other, index);

    if(equippedItemLabel)
    {
        equippedItemLabel->currentPositionSlot = NULL;
        equippedItemLabel = NULL;
    }
}

void InventoryPositionSlot::moveChild(Container* other, Element* child)
{
    bit::Container::moveChild(other, child);
}

void InventoryPositionSlot::removeChild(unsigned int index)
{
    bit::Container::removeChild(index);
    if(equippedItemLabel)
    {
        equippedItemLabel->currentPositionSlot = NULL;
        equippedItemLabel = NULL;
    }
}