#include "LootMenu.hpp"
#include "Hud.hpp"
#include "../ClientRequest.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Network.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"
#include "../Interaction.hpp"
#include "../TileClient.hpp"

LootMenu::LootMenu(Hud* _hud)
    : bit::Container(0, 50, 300, 400, bit::Element::AnchorType::Top, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), hud(_hud), inventory(), isActive(false)
{
    entries = new bit::Label(0, 0, 0, 0, bit::Element::AnchorType::TopLeft);
    entries->setSfFontSize(24);
    entries->setSfFont(hud->journalFont);
    entries->normalColor = sf::Color::White;
    addChild(entries);
}

void LootMenu::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::Container::update(window, gameTime);

    if(isActive)
    {
        canHaveFocus = true;
        opacity = 1;
    }
    else
    {
        canHaveFocus = false;
        opacity = 0;
    }
}

void LootMenu::activate()
{
    isActive = true;
}

void LootMenu::deactivate()
{
    isActive = false;
    clearChildren();
}

void LootMenu::handleInventorySnapshot(bit::ServerPacket &packet, unsigned int tileId)
{
    inventory.handleSnapshot(packet);

    //std::string entry("Loot\n");
    int y = 0;
    for(auto iterator = inventory.itemClients.begin(); iterator != inventory.itemClients.end(); iterator++)
    {
        // Level 1
        ItemClient* i = &iterator->second;
        //entry += "- " + Item::getTitle(i->schema.type) + "\n";

        LootMenu* m = this;
        ///Interaction::Type it;
        ///bit::NetworkHelper::unpackEnum<sf::Uint32, Interaction::Type>(packet, it);
        
        bit::Label* option = new bit::Label(0, y, 0, 0, bit::Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        option->setSfFontSize(24);
        option->setSfFont(hud->journalFont);
        option->normalColor = sf::Color::White;
        option->setSfFontString(Item::getTitle(i->schema.type));
        option->canHaveFocus = true;
        option->paddingRight = 10;
        option->paddingBottom = 10;
        option->onActivate = [m, i] (Element* e){
            LootMenu* mx = m;
            ItemClient* ix = i;
            m->hud->state->serverRequest(
                [mx, ix] (bit::ClientPacket &packet) {
                    packet << sf::Uint32(ClientRequest::TransferItem);
                    packet << sf::Uint32(ix->schema.id);
                },
                [mx, ix] (bit::ServerPacket &packet) {
                }
            );
        };
        addChild(option);

        y += 30;

    }
    //entries->setSfFontString(entry);
}