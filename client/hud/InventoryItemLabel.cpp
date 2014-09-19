#include "InventoryItemLabel.hpp"
#include "InventoryIcon.hpp"
#include "InventoryPositionSlot.hpp"
#include "InventoryEquipmentSlot.hpp"
#include "InventoryLootSlot.hpp"
#include "ActionBarSlot.hpp"
#include "Inventory.hpp"
#include "LootMenu.hpp"
#include "ActionBar.hpp"
#include "Hud.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "../../server/ClientRequest.hpp"

InventoryItemLabel::InventoryItemLabel(Hud* hud, Item::Schema& itemSchema, float relativeX, float relativeY, AnchorType anchorType)
    : bit::Element(relativeX, relativeY, 64, 64, anchorType), hud(hud), itemSchema(itemSchema), icon(NULL), currentEquipmentSlot(NULL), currentPositionSlot(NULL), currentLootSlot(NULL), currentActionSlot(NULL)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    canHaveFocus = true;
    opacity = 0;

    InventoryItemLabel* label = this;
    makeDraggable(hud->state->rogueZombieGame->inputManager, [hud](bit::Draggable *d, Element* e){ hud->hideTooltip(); }, [hud, label] (bit::Draggable* d, Element* e) -> bool
    {
        // Check the equipment slots to see if it is being hovered when the drop occurs
        for(unsigned int i=0; i < hud->inventory->equipmentPanel->childElements.size(); i++)
        {
            // If the slot is not already equipped with the item and its of an acceptable type
            InventoryEquipmentSlot* slot = static_cast<InventoryEquipmentSlot*>(hud->inventory->equipmentPanel->childElements[i]);
            if(slot->isInfocus && slot->equippedItemLabel != e && slot->acceptsLabel(label))
            {
                // Drop the item into the slot
                bool result = label->dropOntoEquipmentSlot(slot);
                label->dropResult(result);
                return result;
            }
        }


        // Check the inventory slots to see if it is being hovered when the drop occurs
        for(unsigned int i=0; i < hud->inventory->positionSlotBoxes.size(); i++)
        {
            // If the slot is not already equipped with the item and its of an acceptable type
            InventoryPositionSlot* slot = static_cast<InventoryPositionSlot*>(hud->inventory->positionSlotBoxes[i]);
            if(slot->isInfocus && slot->equippedItemLabel != e && slot->acceptsLabel(label))
            {
                bool result = label->dropOntoInventorySlot(slot);
                label->dropResult(result);
                return result;
            }
        }
        

        // Check the loot slots to see if it is being hovered when the drop occurs
        for(unsigned int i=0; i < hud->lootMenu->lootSlotBoxes.size(); i++)
        {
            // If the slot is not already equipped with the item and its of an acceptable type
            InventoryLootSlot* slot = static_cast<InventoryLootSlot*>(hud->lootMenu->lootSlotBoxes[i]);
            if(slot->isInfocus && slot->equippedItemLabel != e && slot->acceptsLabel(label))
            {
                bool result = label->dropOntoLootSlot(slot);
                label->dropResult(result);
                return result;
            }
        }

        
        // Check the action bar slots to see if it is being hovered when the drop occurs
        for(unsigned int i=0; i < hud->actionBar->slots.size(); i++)
        {
            // If the slot is not already equipped with the item and its of an acceptable type
            ActionBarSlot* slot = static_cast<ActionBarSlot*>(hud->actionBar->slots[i]);
            if(slot->isInfocus && slot->equippedItemLabel != e && slot->acceptsLabel(label))
            {
                bool result = label->dropOntoActionSlot(slot);
                label->dropResult(result);
                return result;
            }
        }


        // If I am coming from an action bar slot, check if I have dragged outside of a certain range and trigger the removal
        if(label->currentActionSlot)
        {
            if(!hud->actionBar->contains(label->left, label->top))
            {
                label->currentActionSlot->equippedItemLabel = NULL;
                label->currentActionSlot = NULL;
                label->removeFromParent = true;
                return false;
            }
        }

        return false;
    });

    // Icon
    icon = hud->inventoryIconPool.fetch();
    icon->set(std::string("icon_" + Item::getIconName(itemSchema.type)));

    // Tooltip
    InventoryItemLabel* self = this;
    makeHoverable(hud->state->rogueZombieGame->inputManager, [hud, self](bit::Hoverable* h, bit::Element* me) {
        hud->inventory->showItemData(self->itemSchema);
    }, [hud](bit::Hoverable* h, bit::Element* me) {
        hud->inventory->hideItemData();
    });
}

InventoryItemLabel::~InventoryItemLabel()
{
    if(icon)
    {
        if(Hud::destroying)
        {
            delete icon;
        }
        else
        {
            hud->inventoryIconPool.recycle(icon);
        }
    }
}

void InventoryItemLabel::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::Element::updateTargets(window, gameTime);

    float o = opacity;
    icon->color.a = (char)(o * 255);
}

void InventoryItemLabel::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::Element::updateReals(window, gameTime);

    float z = this->draggable->isDragging ? Hud::getDrawDepth(Hud::zindex_iconsDragged) : Hud::getDrawDepth(Hud::zindex_icons);
    icon->position(left, top, z, 64, 64, elementScale);
}


void InventoryItemLabel::dropResult(bool result)
{
    if(result)
    {
        hud->playSlotDenialSound();
    }
}


bool InventoryItemLabel::dropOntoEquipmentSlot(InventoryEquipmentSlot* slot)
{
    // There are three operations that cause this:
    // 1. Dropping from another inventory cell to this equipment slot
    // 2. Dropping from an equipment slot to this equipment slot (such as compatible types like weapons)
    // 3. Dropping from a loot cell into this equipment slot

    // 1. Dropping from another inventory cell to this equipment slot
    if(currentPositionSlot)
    {
        // Visual prediction:
        if(slot->equippedItemLabel)
        {
            InventoryPositionSlot* originSlot = currentPositionSlot;
            InventoryEquipmentSlot* destinationSlot = slot;
            InventoryItemLabel* originLabel = this;
            InventoryItemLabel* destinationLabel = slot->equippedItemLabel;

            originSlot->moveChild(destinationSlot, originLabel);
            destinationSlot->moveChild(originSlot, destinationLabel);
        }
        else
        {
            currentPositionSlot->moveChild(slot, this);
        }

        // Position label within equipment slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Send the request to change the equipment
        Item::Schema schema = itemSchema;
        Hud* hudx = hud;
        hud->state->serverRequest(
            [hudx, schema, slot](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::EquipItemFromInventoryToSlot);
                requestPacket << sf::Uint32(slot->slot);
                requestPacket << sf::Uint32(schema.id);

                hudx->inventory->pendingInventoryRequests++;
            },
            [hudx, schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;

                hudx->inventory->pendingInventoryRequests--;
            }
        );
        return true;
    }

    // 2. Dropping from an equipment slot to this equipment slot (such as compatible types like weapons)
    if(currentEquipmentSlot)
    {
        Character::EquipmentSlot slotA = currentEquipmentSlot->slot;
        Character::EquipmentSlot slotB = slot->slot;

        // If the destination cell has an item already
        if(slot->equippedItemLabel)
        {
            // Make sure the equipment slot can take that item
            if(!currentEquipmentSlot->acceptsLabel(slot->equippedItemLabel))
                return false;
        }

        // Visual prediction:
        if(slot->equippedItemLabel)
        {
            InventoryEquipmentSlot* originSlot = currentEquipmentSlot;
            InventoryEquipmentSlot* destinationSlot = slot;
            InventoryItemLabel* originLabel = this;
            InventoryItemLabel* destinationLabel = slot->equippedItemLabel;

            originSlot->moveChild(destinationSlot, originLabel);
            destinationSlot->moveChild(originSlot, destinationLabel);
        }
        else
        {
            currentEquipmentSlot->moveChild(slot, this);
        }
        
        // Position within the grid slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Networking:
        // 1. Request the server move the item, which includes the swap process
        // 2. This will naturally send one or two item updates which include the positioning information
        // 3. Response is moot
        Item::Schema schema = itemSchema;
        Hud* hudx = hud;
        hud->state->serverRequest(
            [hudx, schema, slot, slotA, slotB](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::SwapEquipmentBySlot);
                requestPacket << sf::Uint32(slotA);
                requestPacket << sf::Uint32(slotB);

                hudx->inventory->pendingInventoryRequests++;
            },
            [hudx, schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;

                hudx->inventory->pendingInventoryRequests--;
            }
        );

        return true;
    }

    // 3. Dropping from a loot cell into this equipment slot
    if(currentLootSlot)
    {
        // Visual prediction:
        if(slot->equippedItemLabel)
        {
            InventoryLootSlot* originSlot = currentLootSlot;
            InventoryEquipmentSlot* destinationSlot = slot;
            InventoryItemLabel* originLabel = this;
            InventoryItemLabel* destinationLabel = slot->equippedItemLabel;

            originSlot->moveChild(destinationSlot, originLabel);
            destinationSlot->moveChild(originSlot, destinationLabel);
        }
        else
        {
            currentLootSlot->moveChild(slot, this);
        }

        // Position label within equipment slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Send the request to change the equipment
        Item::Schema schema = itemSchema;
        Hud* hudx = hud;
        hud->state->serverRequest(
            [hudx, schema, slot](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::EquipItemFromLootToSlot);
                requestPacket << sf::Uint32(slot->slot);
                requestPacket << sf::Uint32(schema.id);

                hudx->inventory->pendingInventoryRequests++;
            },
            [hudx, schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;
                hudx->lootMenu->syncInventory();

                hudx->inventory->pendingInventoryRequests--;
            }
        );
        return true;
    }

    return false;
}


bool InventoryItemLabel::dropOntoInventorySlot(InventoryPositionSlot* slot)
{
    // There are three operations that cause this:
    // 1. Dropping from another inventory cell to this inventory cell
    // 2. Dropping from an equipment slot to this inventory cell
    // 3. Dropping from a loot cell into this inventory cell

    // 1. Dropping from another inventory cell to this inventory cell
    if(currentPositionSlot)
    {
        // Visual prediction:
        if(slot->equippedItemLabel)
        {
            InventoryPositionSlot* originSlot = currentPositionSlot;
            InventoryPositionSlot* destinationSlot = slot;
            InventoryItemLabel* originLabel = this;
            InventoryItemLabel* destinationLabel = slot->equippedItemLabel;

            originSlot->moveChild(destinationSlot, originLabel);
            destinationSlot->moveChild(originSlot, destinationLabel);
        }
        else
        {
            currentPositionSlot->moveChild(slot, this);
        }
        

        // Position within the grid slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Networking:
        // 1. Request the server move the item, which includes the swap process
        // 2. This will naturally send one or two item updates which include the positioning information
        // 3. Response is moot
        Item::Schema schema = itemSchema;
        Hud* hudx = hud;
        hud->state->serverRequest(
            [hudx, schema, slot](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::MoveInventoryItemToPosition);
                requestPacket << sf::Uint32(schema.id);
                requestPacket << sf::Uint32(slot->position);

                hudx->inventory->pendingInventoryRequests++;
            },
            [hudx, schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;

                hudx->inventory->pendingInventoryRequests--;
            }
        );

        return true;
    }

    // 2. Dropping from an equipment slot to this inventory cell
    if(currentEquipmentSlot)
    {
        Character::EquipmentSlot equipmentSlot = currentEquipmentSlot->slot;

        // If the destination cell has an item already
        if(slot->equippedItemLabel)
        {
            // Make sure the equipment slot can take that item
            if(!currentEquipmentSlot->acceptsLabel(slot->equippedItemLabel))
                return false;
        }

        // Visual prediction:
        if(slot->equippedItemLabel)
        {
            InventoryEquipmentSlot* originSlot = currentEquipmentSlot;
            InventoryPositionSlot* destinationSlot = slot;
            InventoryItemLabel* originLabel = this;
            InventoryItemLabel* destinationLabel = slot->equippedItemLabel;

            originSlot->moveChild(destinationSlot, originLabel);
            destinationSlot->moveChild(originSlot, destinationLabel);
        }
        else
        {
            currentEquipmentSlot->moveChild(slot, this);
        }
        
        // Position within the grid slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Networking:
        // 1. Request the server move the item, which includes the swap process
        // 2. This will naturally send one or two item updates which include the positioning information
        // 3. Response is moot
        Item::Schema schema = itemSchema;
        Hud* hudx = hud;
        hud->state->serverRequest(
            [hudx, schema, slot, equipmentSlot](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::MoveEquippedItemToInventoryPosition);
                requestPacket << sf::Uint32(equipmentSlot);
                requestPacket << sf::Uint32(slot->position);

                hudx->inventory->pendingInventoryRequests++;
            },
            [hudx, schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;

                hudx->inventory->pendingInventoryRequests--;
            }
        );

        return true;
    }

    // 3. Dropping from a loot cell into this inventory cell
    if(currentLootSlot)
    {
        // Visual prediction:
        if(slot->equippedItemLabel)
        {
            InventoryLootSlot* originSlot = currentLootSlot;
            InventoryPositionSlot* destinationSlot = slot;
            InventoryItemLabel* originLabel = this;
            InventoryItemLabel* destinationLabel = slot->equippedItemLabel;

            originSlot->moveChild(destinationSlot, originLabel);
            destinationSlot->moveChild(originSlot, destinationLabel);
        }
        else
        {
            currentLootSlot->moveChild(slot, this);
        }

        // Position within the grid slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Networking:
        // 1. Request the server move the item, which includes the swap process
        // 2. This will naturally send one or two item updates which include the positioning information
        // 3. Response is moot
        Item::Schema schema = itemSchema;
        Hud* hudx = hud;
        hud->state->serverRequest(
            [hudx, schema, slot](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::MoveLootItemToInventoryPosition);
                requestPacket << sf::Uint32(schema.id);
                requestPacket << sf::Uint32(slot->position);

                hudx->inventory->pendingInventoryRequests++;
            },
            [hudx, schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;
                hudx->lootMenu->syncInventory();

                hudx->inventory->pendingInventoryRequests--;
            }
        );

        return true;
    }

    return false;
}


bool InventoryItemLabel::dropOntoLootSlot(InventoryLootSlot* slot)
{
    // There are three operations that cause this:
    // 1. Dropping from another loot cell to this loot cell
    // 2. Dropping from an inventory cell into this loot cell
    // 3. Dropping from an equipment slot to this loot cell

    // 1. Dropping from another loot cell to this loot cell
    if(currentLootSlot && currentLootSlot != slot)
    {
        // Visual prediction:
        if(slot->equippedItemLabel)
        {
            InventoryLootSlot* originSlot = currentLootSlot;
            InventoryLootSlot* destinationSlot = slot;
            InventoryItemLabel* originLabel = this;
            InventoryItemLabel* destinationLabel = slot->equippedItemLabel;

            originSlot->moveChild(destinationSlot, originLabel);
            destinationSlot->moveChild(originSlot, destinationLabel);
        }
        else
        {
            currentLootSlot->moveChild(slot, this);
        }

        // Position within the grid slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Networking:
        // 1. Request the server move the item, which includes the swap process
        // 2. This will naturally send one or two item updates which include the positioning information
        // 3. Response is moot
        Item::Schema schema = itemSchema;
        Hud* hudx = hud;
        hud->state->serverRequest(
            [hudx, schema, slot](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::MoveLootItemToLootPosition);
                requestPacket << sf::Uint32(schema.id);
                requestPacket << sf::Uint32(slot->position);

                hudx->inventory->pendingInventoryRequests++;
            },
            [hudx, schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;
                hudx->lootMenu->syncInventory();

                hudx->inventory->pendingInventoryRequests--;
            }
        );

        return true;
    }

    // 2. Dropping from an inventory cell into this loot cell
    if(currentPositionSlot)
    {
        // Visual prediction:
        if(slot->equippedItemLabel)
        {
            InventoryPositionSlot* originSlot = currentPositionSlot;
            InventoryLootSlot* destinationSlot = slot;
            InventoryItemLabel* originLabel = this;
            InventoryItemLabel* destinationLabel = slot->equippedItemLabel;

            originSlot->moveChild(destinationSlot, originLabel);
            destinationSlot->moveChild(originSlot, destinationLabel);
        }
        else
        {
            currentPositionSlot->moveChild(slot, this);
        }

        // Position within the grid slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Networking:
        // 1. Request the server move the item, which includes the swap process
        // 2. This will naturally send one or two item updates which include the positioning information
        // 3. Response is moot
        Item::Schema schema = itemSchema;
        Hud* hudx = hud;
        hud->state->serverRequest(
            [hudx, schema, slot](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::MoveInventoryItemToLootPosition);
                requestPacket << sf::Uint32(schema.id);
                requestPacket << sf::Uint32(slot->position);

                hudx->inventory->pendingInventoryRequests++;
            },
            [hudx, schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;
                hudx->lootMenu->syncInventory();

                hudx->inventory->pendingInventoryRequests--;
            }
        );

        return true;
    }

    // 3. Dropping from an equipment slot to this loot cell
    if(currentEquipmentSlot)
    {
        Character::EquipmentSlot equipmentSlot = currentEquipmentSlot->slot;

        // If the destination cell has an item already
        if(slot->equippedItemLabel)
        {
            // Make sure the equipment slot can take that item
            if(!currentEquipmentSlot->acceptsLabel(slot->equippedItemLabel))
                return false;
        }

        // Visual prediction:
        if(slot->equippedItemLabel)
        {
            InventoryEquipmentSlot* originSlot = currentEquipmentSlot;
            InventoryLootSlot* destinationSlot = slot;
            InventoryItemLabel* originLabel = this;
            InventoryItemLabel* destinationLabel = slot->equippedItemLabel;

            originSlot->moveChild(destinationSlot, originLabel);
            destinationSlot->moveChild(originSlot, destinationLabel);
        }
        else
        {
            currentEquipmentSlot->moveChild(slot, this);
        }
        
        // Position within the grid slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Networking:
        // 1. Request the server move the item, which includes the swap process
        // 2. This will naturally send one or two item updates which include the positioning information
        // 3. Response is moot
        Item::Schema schema = itemSchema;
        Hud* hudx = hud;
        hud->state->serverRequest(
            [hudx, schema, slot, equipmentSlot](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::MoveEquippedItemToLootPosition);
                requestPacket << sf::Uint32(equipmentSlot);
                requestPacket << sf::Uint32(slot->position);

                hudx->inventory->pendingInventoryRequests++;
            },
            [hudx, schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;
                hudx->lootMenu->syncInventory();

                hudx->inventory->pendingInventoryRequests--;
            }
        );

        return true;
    }

    return false;
}



bool InventoryItemLabel::dropOntoActionSlot(ActionBarSlot* slot)
{
    // There are one acceptable operations that cause this:
    // 1. Dropping from an inventory cell into this action bar cell
    // 2. Dropping from another action cell into this action cell

    // 1. Dropping from an inventory cell into this action bar cell
    if(currentPositionSlot)
    {
        // Visual prediction:
        // existing item label
        if(slot->equippedItemLabel)
        {
            // Delete the existing child
            slot->removeItemLabel();
        }

        // Create the new clone child
        InventoryItemLabel* newItemLabel = new InventoryItemLabel(hud, this->itemSchema, 0, 0, AnchorType::TopLeft);
        slot->addItemLabel(newItemLabel);

        // RePosition within the grid slot self and also return true for the drag release
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Actioneering:
        // This will set up the new item label to listen to the click event that will set the targetting system in motion
        return true;
    }


    // 2. Dropping from another action cell into this action cell
    if(currentActionSlot && currentActionSlot != slot)
    {
        // Visual prediction:
        // If something exists, swap them
        if(slot->equippedItemLabel)
        {
            ActionBarSlot* originSlot = currentActionSlot;
            ActionBarSlot* destinationSlot = slot;
            InventoryItemLabel* originLabel = this;
            InventoryItemLabel* destinationLabel = slot->equippedItemLabel;

            originSlot->moveChild(destinationSlot, originLabel);
            destinationSlot->moveChild(originSlot, destinationLabel);
        }
        // If nothing exists just move me there
        else
        {
            currentActionSlot->moveChild(slot, this);
        }

        // Position within the grid slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Actioneering not required because it should carry the action operations along with it via item
        return true;
    }

    return false;
}