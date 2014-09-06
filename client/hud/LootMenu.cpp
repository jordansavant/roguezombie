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
    : HudMenu(_hud, 50, 0, 618, 720, bit::Element::AnchorType::Left, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)),
      inventory(), isActive(false), tileId(0)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    managesOpacity = true;
    opacity = 0;
    entries = new bit::Label(0, 0, 0, 0, bit::Element::AnchorType::TopLeft);
    entries->setSfFontSize(24);
    entries->setSfFont(hud->journalFont);
    entries->normalColor = sf::Color::White;
    addChild(entries);

    // INVENTORY SLOTS
    int y = 72;
    int width = 64;
    int pad = 8;
    int x = pad;
    for(unsigned int j=0; j < 20; j++)
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
    immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, -targetWidth, 0));
    immediateEffect(new bit::FadeEffect(150, 0));
}

void LootMenu::show()
{
    canHaveFocus = true;
    isShown = true;
    clearEffects();
    relativePosition.x = -targetWidth;
    immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, targetWidth, 0));
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