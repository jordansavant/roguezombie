#include "InventoryItemLabel.hpp"
#include "InventoryEquipmentSlot.hpp"
#include "Inventory.hpp"
#include "Hud.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "../ClientRequest.hpp"

InventoryItemLabel::InventoryItemLabel(Inventory* inventory, ItemClient* item, float relativeX, float relativeY, AnchorType anchorType)
    : bit::Label(relativeX, relativeY, 0, 0, anchorType), inventory(inventory), item(item)
{
    setSfFontSize(24);
    setSfFont(inventory->hud->journalFont);
    normalColor = sf::Color::White;
    setSfFontString(std::string("- " + Item::getTitle(item->schema.type)));
    canHaveFocus = true;
    paddingRight = 10;
    paddingBottom = 10;

    makeDraggable(inventory->hud->state->rogueZombieGame->inputManager);
    draggable->onDragStop = [inventory, item] (bit::Draggable* d, Element* e) -> bool
    {
        // If dropping into an equipment slot
        for(unsigned int i=0; i < inventory->equipmentPanel->childElements.size(); i++)
        {
            // If the slot is not already equipped with the item
            InventoryEquipmentSlot* slot = static_cast<InventoryEquipmentSlot*>(inventory->equipmentPanel->childElements[i]);
            if(slot->isInfocus && slot->equippedItemLabel != e)
            {
                bit::Container* currentParent = static_cast<bit::Container*>(e->parentElement);
                currentParent->moveChild(slot, e);
                e->relativePosition.x = 0;
                e->relativePosition.y = 0;

                ItemClient* itemx = item;
                if(slot == inventory->headBox)
                {
                    // Send the request to change the head equipment
                    inventory->hud->state->serverRequest(
                        [itemx](bit::ClientPacket& requestPacket)
                        {
                            requestPacket <<sf::Uint32(ClientRequest::EquipHead);
                            requestPacket << sf::Uint32(itemx->schema.id);
                        },
                        [itemx](bit::ServerPacket& responsePacket)
                        {
                            bool success;
                            responsePacket >> success;
                        }
                    );
                }

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
                    if(slot == inventory->headBox)
                    {
                        inventory->hud->state->serverRequest(
                            [](bit::ClientPacket& requestPacket)
                            {
                                requestPacket <<sf::Uint32(ClientRequest::UnequipHead);
                            },
                            [](bit::ServerPacket& responsePacket)
                            {
                            }
                        );
                    }
                }
            }
            bit::Container* p = static_cast<bit::Container*>(e->parentElement);
            p->moveChild(inventory->inventoryPanel, e);
            e->relativePosition.x = 0;
            e->relativePosition.y = 0;
            return true;
        }

        return false;
    };
}