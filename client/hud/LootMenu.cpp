#include "LootMenu.hpp"
#include "Hud.hpp"
#include "../../server/ClientRequest.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Network.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"
#include "../../server/Interaction.hpp"
#include "../TileClient.hpp"
#include "InventoryLootSlot.hpp"
#include "InventoryItemLabel.hpp"

LootMenu::LootMenu(Hud* _hud)
    : HudMenu(_hud, 50, 0, 296, 728, bit::Element::AnchorType::Right, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)),
      inventory(), isActive(false), tileId(0)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    managesOpacity = true;
    opacity = 0;

    // INVENTORY SLOTS
    int y = 8;
    int width = 64;
    int pad = 8;
    int x = pad;
    for(unsigned int j=0; j < 40; j++)
    {
        if(x + width + pad > targetWidth)
        {
            y += width + pad;
            x = pad;
        }
        InventoryLootSlot* slot = new InventoryLootSlot(hud, j, x, y, width, width, bit::Element::AnchorType::TopLeft);
        addChild(slot);
        lootSlotBoxes.push_back(slot);
        x += width + pad;
    }
}

void LootMenu::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::Container::update(window, gameTime);
}

void LootMenu::activate()
{
    isActive = true;
    show();
}

void LootMenu::deactivate()
{
    isActive = false;
    tileId = 0;
    hide();
}

void LootMenu::hide()
{
    canHaveFocus = false;
    isShown = false;
    clearEffects();
    relativePosition.x = 0;
    immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, targetWidth + 700, 0));
    immediateEffect(new bit::FadeEffect(150, 0));
}

void LootMenu::show()
{
    canHaveFocus = true;
    isShown = true;
    clearEffects();
    relativePosition.x = targetWidth;
    immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, -targetWidth - 700, 0));
    immediateEffect(new bit::FadeEffect(300, Hud::popupOpacity));
}

void LootMenu::syncInventory()
{
    // If no tile, clear
    if(tileId == 0)
    {
        for(unsigned int i=0; i < lootSlotBoxes.size(); i++)
        {
            lootSlotBoxes[i]->clearChildren();
        }
        return;
    }

    // If there is a tile from the current session, open it
    LootMenu* m = this;
    unsigned int tileIdx = tileId;
    hud->state->serverRequest(
        [tileIdx, m] (bit::ClientPacket &packet)
        {
            packet << sf::Uint32(ClientRequest::ProcessInteractionForBodyOnTile);
            packet << sf::Uint32(Interaction::Type::OpenInventory);
            packet << sf::Uint32(tileIdx);
        },
        [tileIdx, m] (bit::ServerPacket &packet)
        {
            m->hud->state->handleInteractionResponse(tileIdx, Interaction::Type::OpenInventory, packet);
        }
    );
}

void LootMenu::handleInventorySnapshot(bit::ServerPacket &packet, unsigned int tileId)
{
    this->tileId = tileId;
    inventory.handleSnapshot(packet);

    // Check if I can avoid rebuilding the GUI elements
    bool match = true;

    // Check that all items on the inventory are correct in the GUI
    for(auto iterator = inventory.itemClients.begin(); iterator != inventory.itemClients.end(); iterator++)
    {
        ItemClient* item = &iterator->second;
        unsigned int position = item->schema.position;
        unsigned int itemId = item->schema.id;
        if(position < lootSlotBoxes.size())
        {
            if(lootSlotBoxes[position]->equippedItemLabel == NULL || lootSlotBoxes[position]->equippedItemLabel->itemSchema.id != itemId)
            {
                match = false;
                break;
            }
        }
    }

    // Check that all items in the GUI match the items in the player
    if(match)
    {
        for(unsigned int i=0; i < lootSlotBoxes.size(); i++)
        {
            Item::Schema* labelItem = lootSlotBoxes[i]->equippedItemLabel ? &lootSlotBoxes[i]->equippedItemLabel->itemSchema : NULL;
            Item::Schema* invenIten = NULL;

            if(labelItem)
            {
                auto result = inventory.itemClients.find(labelItem->id);
                if(result != inventory.itemClients.end())
                {
                    invenIten = &result->second.schema;
                }
            }

            if(labelItem)
            {
                if(invenIten == NULL || labelItem->position != invenIten->position)
                {
                    match = false;
                    break;
                }
            }
        }
    }

    if(match)
        return;

    // clean up
    for(unsigned int i=0; i < lootSlotBoxes.size(); i++)
    {
        lootSlotBoxes[i]->clearChildren();
    }

    unsigned int i=0;
    for(auto iterator = inventory.itemClients.begin(); iterator != inventory.itemClients.end(); iterator++)
    {
        ItemClient* item = &iterator->second;
        InventoryItemLabel* option = new InventoryItemLabel(hud, item, 0, 0, bit::Element::AnchorType::TopLeft);
        lootSlotBoxes[item->schema.position]->addChild(option);
        i++;
    }
}