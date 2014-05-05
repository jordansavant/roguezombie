#include "InventoryItemLabel.hpp"
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
            if(inventory->equipmentPanel->childElements[i]->isInfocus)
            {
                bit::Container* c = static_cast<bit::Container*>(inventory->equipmentPanel->childElements[i]);
                bit::Container* p = static_cast<bit::Container*>(e->parentElement);
                p->moveChild(c, e);
                e->relativePosition.x = 0;
                e->relativePosition.y = 0;


                ItemClient* itemx = item;
                if(inventory->equipmentPanel->childElements[i] == inventory->headBox)
                {
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
            bit::Container* p = static_cast<bit::Container*>(e->parentElement);
            p->moveChild(inventory->inventoryPanel, e);
            e->relativePosition.x = 0;
            e->relativePosition.y = 0;
            return true;
        }

        return false;
    };
}