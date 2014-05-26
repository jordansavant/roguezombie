#include "InventoryItemLabel.hpp"
#include "InventoryEquipmentSlot.hpp"
#include "Inventory.hpp"
#include "Hud.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "../ClientRequest.hpp"

InventoryItemLabel::InventoryItemLabel(Inventory* inventory, ItemClient* item, float relativeX, float relativeY, AnchorType anchorType)
    : bit::Label(relativeX, relativeY, 0, 0, anchorType), inventory(inventory), itemSchema(item->schema), currentEquipmentSlot(NULL)
{
    setSfFontSize(24);
    setSfFont(inventory->hud->journalFont);
    normalColor = sf::Color::White;
    focusedColor = sf::Color::Red;
    setSfFontString(std::string(Item::getTitle(item->schema.type)));
    canHaveFocus = true;
    paddingRight = 10;
    opacity = 0;
    paddingBottom = 10;

    InventoryItemLabel* label = this;
    makeDraggable(inventory->hud->state->rogueZombieGame->inputManager, NULL, [inventory, item, label] (bit::Draggable* d, Element* e) -> bool
    {
        // If dropping into an equipment slot
        for(unsigned int i=0; i < inventory->equipmentPanel->childElements.size(); i++)
        {
            InventoryEquipmentSlot* slot = static_cast<InventoryEquipmentSlot*>(inventory->equipmentPanel->childElements[i]);

            // If the slot is not already equipped with the item and its of an acceptable type
            if(slot->isInfocus && slot->equippedItemLabel != e && slot->acceptsLabel(label))
            {
                // If the current slot had an item, move it back to the inventory
                if(slot->equippedItemLabel)
                {
                    slot->removeItemLabel();
                }

                // Move this item from my current parent to the slot
                bit::Container* currentParent = static_cast<bit::Container*>(e->parentElement);
                currentParent->moveChild(slot, e);

                e->relativePosition.x = 0;
                e->relativePosition.y = 0;

                // Send the request to change the equipment
                ItemClient* itemx = item;
                inventory->hud->state->serverRequest(
                    [itemx, slot](bit::ClientPacket& requestPacket)
                    {
                        requestPacket << sf::Uint32(ClientRequest::EquipItem);
                        requestPacket << sf::Uint32(slot->slot);
                        requestPacket << sf::Uint32(itemx->schema.id);
                    },
                    [itemx](bit::ServerPacket& responsePacket)
                    {
                        bool success;
                        responsePacket >> success;
                    }
                );

                return true;
            }
        }
        
        // If dropping into the inventory
        if(inventory->inventoryPanel->isInfocus)
        {
            // See which equipment slot it came from
            for(unsigned int i=0; i < inventory->equipmentPanel->childElements.size(); i++)
            {
                InventoryEquipmentSlot* slot = static_cast<InventoryEquipmentSlot*>(inventory->equipmentPanel->childElements[i]);
                if(slot->equippedItemLabel == e)
                {
                    // If it did come from an equipment slot send the unequip packet
                    inventory->hud->state->serverRequest(
                        [slot](bit::ClientPacket& requestPacket)
                        {
                            requestPacket <<sf::Uint32(ClientRequest::UnequipItem);
                            requestPacket << sf::Uint32(slot->slot);
                        },
                        [](bit::ServerPacket& responsePacket)
                        {
                        }
                    );
                }
            }
            bit::Container* p = static_cast<bit::Container*>(e->parentElement);
            p->moveChild(inventory->inventoryPanel, e);
            e->relativePosition.x = 0;
            e->relativePosition.y = 0;
            return true;
        }

        return false;
    });
}