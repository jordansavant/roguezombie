#pragma once
#ifndef RZ_INVENTORYPOSITIONSLOT_H
#define RZ_INVENTORYPOSITIONSLOT_H

#include "Inventory.hpp"
#include "Frame.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../items/ItemClient.hpp"
#include "../../server/Character.hpp"

class Hud;
class InventoryItemLabel;

class InventoryPositionSlot : public Frame
{
public:

    InventoryPositionSlot(Hud* hud, unsigned int position, Inventory* inventory, float relativeX, float relativeY, float width, float height, AnchorType anchorType);

    Inventory* inventory;
    InventoryItemLabel* equippedItemLabel;
    unsigned int position;

    void addItemLabel(InventoryItemLabel* label);

    void removeItemLabel();

    bool acceptsLabel(InventoryItemLabel* label);

    virtual unsigned int addChild(Element* child);

    virtual void moveChild(Container* other, unsigned int childIndex);

    virtual void moveChild(Container* other, Element* child);

    virtual void removeChild(unsigned int childIndex);
};

#endif