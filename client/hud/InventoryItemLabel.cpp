#include "InventoryItemLabel.hpp"
#include "InventoryPositionSlot.hpp"
#include "InventoryEquipmentSlot.hpp"
#include "InventoryLootSlot.hpp"
#include "Inventory.hpp"
#include "LootMenu.hpp"
#include "Hud.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "../../server/ClientRequest.hpp"

InventoryItemLabel::InventoryItemLabel(Hud* hud, ItemClient* item, float relativeX, float relativeY, AnchorType anchorType)
    : bit::Label(relativeX, relativeY, 0, 0, anchorType), hud(hud), itemSchema(item->schema), currentEquipmentSlot(NULL), currentPositionSlot(NULL), currentLootSlot(NULL)
{
    setSfFontSize(24);
    setSfFont(hud->journalFont);
    normalColor = sf::Color(0, 255, 0);
    focusedColor = sf::Color::White;
    setSfFontString(std::string(Item::getTitle(item->schema.type)));
    canHaveFocus = true;
    paddingRight = 10;
    opacity = 0;
    paddingBottom = 10;

    InventoryItemLabel* label = this;
    makeDraggable(hud->state->rogueZombieGame->inputManager, NULL, [hud, item, label] (bit::Draggable* d, Element* e) -> bool
    {
        // Check the equipment slots to see if it is being hovered when the drop occurs
        for(unsigned int i=0; i < hud->inventory->equipmentPanel->childElements.size(); i++)
        {
            // If the slot is not already equipped with the item and its of an acceptable type
            InventoryEquipmentSlot* slot = static_cast<InventoryEquipmentSlot*>(hud->inventory->equipmentPanel->childElements[i]);
            if(slot->isInfocus && slot->equippedItemLabel != e && slot->acceptsLabel(label))
            {
                // Drop the item into the slot
                return label->dropOntoEquipmentSlot(slot);
            }
        }


        // Check the inventory slots to see if it is being hovered when the drop occurs
        for(unsigned int i=0; i < hud->inventory->positionSlotBoxes.size(); i++)
        {
            // If the slot is not already equipped with the item and its of an acceptable type
            InventoryPositionSlot* slot = static_cast<InventoryPositionSlot*>(hud->inventory->positionSlotBoxes[i]);
            if(slot->isInfocus && slot->equippedItemLabel != e && slot->acceptsLabel(label))
            {
                return label->dropOntoInventorySlot(slot);
            }
        }
        

        // Check the loot slots to see if it is being hovered when the drop occurs
        for(unsigned int i=0; i < hud->lootMenu->lootSlotBoxes.size(); i++)
        {
            // If the slot is not already equipped with the item and its of an acceptable type
            InventoryLootSlot* slot = static_cast<InventoryLootSlot*>(hud->lootMenu->lootSlotBoxes[i]);
            if(slot->isInfocus && slot->equippedItemLabel != e && slot->acceptsLabel(label))
            {
                return label->dropOntoLootSlot(slot);
            }
        }

    });
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
        // If the targetted slot had an item, remove it so it returns to the inventory
        if(slot->equippedItemLabel)
            slot->removeItemLabel();

        // Move this item from my current parent to the slot
        currentPositionSlot->moveChild(slot, this);

        // Position label within equipment slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Send the request to change the equipment
        Item::Schema schema = itemSchema;
        hud->state->serverRequest(
            [schema, slot](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::EquipItemFromInventoryToSlot);
                requestPacket << sf::Uint32(slot->slot);
                requestPacket << sf::Uint32(schema.id);
            },
            [schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;
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
            
            if(slot->equippedItemLabel)
                slot->removeItemLabel();
        }

        // Visual prediction:
        currentEquipmentSlot->moveChild(slot, this);
        
        // Position within the grid slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Networking:
        // 1. Request the server move the item, which includes the swap process
        // 2. This will naturally send one or two item updates which include the positioning information
        // 3. Response is moot
        Item::Schema schema = itemSchema;
        hud->state->serverRequest(
            [schema, slot, slotA, slotB](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::SwapEquipmentBySlot);
                requestPacket << sf::Uint32(slotA);
                requestPacket << sf::Uint32(slotB);
            },
            [schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;
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
        // 1. Remove it from the current parent
        // 2. Take what is in the new parent, if anything
        // 3. Add this to the new parent
        // 4. Add the other to the old parent, if anything
        currentPositionSlot->moveChild(slot, this);
        

        // Position within the grid slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Networking:
        // 1. Request the server move the item, which includes the swap process
        // 2. This will naturally send one or two item updates which include the positioning information
        // 3. Response is moot
        Item::Schema schema = itemSchema;
        hud->state->serverRequest(
            [schema, slot](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::MoveInventoryItemToPosition);
                requestPacket << sf::Uint32(schema.id);
                requestPacket << sf::Uint32(slot->position);
            },
            [schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;
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
        currentEquipmentSlot->moveChild(slot, this);
        
        // Position within the grid slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Networking:
        // 1. Request the server move the item, which includes the swap process
        // 2. This will naturally send one or two item updates which include the positioning information
        // 3. Response is moot
        Item::Schema schema = itemSchema;
        hud->state->serverRequest(
            [schema, slot, equipmentSlot](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::MoveEquippedItemToInventoryPosition);
                requestPacket << sf::Uint32(equipmentSlot);
                requestPacket << sf::Uint32(slot->position);
            },
            [schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;
            }
        );

        return true;
    }

    // 3. Dropping from a loot cell into this inventory cell
    if(currentLootSlot)
    {
        currentLootSlot->moveChild(slot, this);

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
            [schema, slot](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::MoveLootItemToInventoryPosition);
                requestPacket << sf::Uint32(schema.id);
                requestPacket << sf::Uint32(slot->position);
            },
            [hudx, schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;
                hudx->lootMenu->syncInventory();
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
    if(currentLootSlot)
    {
        // not supported yet
        return false;
    }

    // 2. Dropping from an inventory cell into this loot cell
    if(currentPositionSlot)
    {
        // Visual prediction:
        // 1. Remove it from the current parent
        // 2. Take what is in the new parent, if anything
        // 3. Add this to the new parent
        // 4. Add the other to the old parent, if anything
        currentPositionSlot->moveChild(slot, this);
        

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
            [schema, slot](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::MoveInventoryItemToLootPosition);
                requestPacket << sf::Uint32(schema.id);
                requestPacket << sf::Uint32(slot->position);
            },
            [hudx, schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;
                hudx->lootMenu->syncInventory();
            }
        );

        return true;
    }

    return false;

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
        currentEquipmentSlot->moveChild(slot, this);
        
        // Position within the grid slot
        relativePosition.x = 0;
        relativePosition.y = 0;

        // Networking:
        // 1. Request the server move the item, which includes the swap process
        // 2. This will naturally send one or two item updates which include the positioning information
        // 3. Response is moot
        Item::Schema schema = itemSchema;
        hud->state->serverRequest(
            [schema, slot, equipmentSlot](bit::ClientPacket& requestPacket)
            {
                requestPacket << sf::Uint32(ClientRequest::MoveEquippedItemToInventoryPosition);
                requestPacket << sf::Uint32(equipmentSlot);
                requestPacket << sf::Uint32(slot->position);
            },
            [schema](bit::ServerPacket& responsePacket)
            {
                bool success;
                responsePacket >> success;
            }
        );

        return true;
    }

    return false;
}