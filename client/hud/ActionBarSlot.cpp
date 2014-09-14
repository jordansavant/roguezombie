#include "InventoryItemLabel.hpp"
#include "ActionBarSlot.hpp"
#include "Inventory.hpp"
#include "Hud.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "../../server/ClientRequest.hpp"

ActionBarSlot::ActionBarSlot(Hud* hud, float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : Frame(hud, relativeX, relativeY, width, height, anchorType, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)),
      equippedItemLabel(NULL)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    managesOpacity = true;
}

void ActionBarSlot::addItemLabel(InventoryItemLabel* label)
{
    label->currentActionSlot = this;
    addChild(label);
}

void ActionBarSlot::removeItemLabel()
{
    if(equippedItemLabel)
    {
        equippedItemLabel->currentActionSlot = NULL;
        equippedItemLabel = NULL;
    }
    clearChildren();
}

bool ActionBarSlot::acceptsLabel(InventoryItemLabel* label)
{
    return true;
}

unsigned int ActionBarSlot::addChild(Element* child)
{
    InventoryItemLabel* label = static_cast<InventoryItemLabel*>(child);
    equippedItemLabel = label;
    equippedItemLabel->currentActionSlot = this;
    return bit::Container::addChild(child);
}

void ActionBarSlot::moveChild(Container* other, unsigned int index)
{
    bit::Container::moveChild(other, index);

    if(equippedItemLabel)
    {
        equippedItemLabel->currentActionSlot = NULL;
        equippedItemLabel = NULL;
    }
}

void ActionBarSlot::moveChild(Container* other, Element* child)
{
    bit::Container::moveChild(other, child);
}

void ActionBarSlot::removeChild(unsigned int index)
{
    bit::Container::removeChild(index);
    if(equippedItemLabel)
    {
        equippedItemLabel->currentActionSlot = NULL;
        equippedItemLabel = NULL;
    }
}