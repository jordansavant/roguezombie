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
    entries->setSfFontString(std::string("Loot\n"));
}

void LootMenu::deactivate()
{
    isActive = false;
    entries->setSfFontString(std::string(""));
}

void LootMenu::handleInventorySnapshot(bit::ServerPacket &packet, unsigned int tileId)
{
    inventory.handleSnapshot(packet);

    std::string entry("Loot\n");
    for(auto iterator = inventory.itemClients.begin(); iterator != inventory.itemClients.end(); iterator++)
    {
        // Level 1
        ItemClient* i = &iterator->second;
        entry += "- " + Item::getTitle(i->schema.type) + "\n";
    }
    entries->setSfFontString(entry);
}