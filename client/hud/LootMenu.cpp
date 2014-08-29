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
    : HudMenu(_hud, 50, 0, 618, 720, bit::Element::AnchorType::Left, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), inventory(), isActive(false)
{
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

void LootMenu::handleInventorySnapshot(bit::ServerPacket &packet, unsigned int tileId)
{
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
    return;

    int y = 10;
    int x = 20;
    int fontSize = 24;
    clearChildren();

    bit::Label* title = new bit::Label(x, y, 0, 0, bit::Element::AnchorType::TopLeft);
    title->setSfFontSize(fontSize);
    title->setSfFont(hud->journalFont);
    title->normalColor = sf::Color(0, 255, 0);
    title->setSfFontString(std::string("LOOT:"));
    addChild(title);
    y += 40;

    for(auto iterator = inventory.itemClients.begin(); iterator != inventory.itemClients.end(); iterator++)
    {
        ItemClient* i = &iterator->second;

        LootMenu* m = this;
        bit::Label* option = new bit::Label(x, y, 0, 0, bit::Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        option->setSfFontSize(fontSize);
        option->setSfFont(hud->journalFont);
        option->normalColor = sf::Color(0, 255, 0);
        option->focusedColor = sf::Color::White;
        option->setSfFontString(std::string(Item::getTitle(i->schema.type)));
        option->canHaveFocus = true;
        option->paddingRight = 10;
        option->paddingBottom = 10;
        option->onActivate = [m, i, tileId] (Element* e){

            LootMenu* mx = m;
            ItemClient* ix = i;
            unsigned int tileIdx = tileId;

            // If an item is clicked send a request to the server to transfer the item
            m->hud->state->serverRequest(

                [mx, ix, tileIdx] (bit::ClientPacket &packet)
                {
                    packet << sf::Uint32(ClientRequest::TransferItemFromCounterpartyToParty);
                    packet << sf::Uint32(ix->schema.id);
                },

                [mx, ix, tileIdx] (bit::ServerPacket &packet)
                {
                    bool success;
                    packet >> success;
                    
                    LootMenu* mxy = mx;
                    ItemClient* ixy = ix;
                    unsigned int tileIdxy = tileIdx;

                    // After the transfer has been issued, request a resync of the objects inventory
                    mx->hud->state->serverRequest(

                        [tileIdxy, mxy] (bit::ClientPacket &packet)
                        {
                            packet << sf::Uint32(ClientRequest::ProcessInteractionForBodyOnTile);
                            packet << sf::Uint32(Interaction::Type::OpenInventory);
                            packet << sf::Uint32(tileIdxy);
                        },

                        [tileIdxy, mxy] (bit::ServerPacket &packet)
                        {
                            mxy->hud->state->handleInteractionResponse(tileIdxy, Interaction::Type::OpenInventory, packet);
                        }
                    );
                }
            );
        };
        addChild(option);

        y += 30;
    }
}