#include "Inventory.hpp"
#include "Hud.hpp"
#include "../../bitengine/Input.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"

Inventory::Inventory(Hud* _hud)
    : HudMenu(_hud)
{
    entries = new bit::Label(0, 0, 0, 0, bit::Element::AnchorType::TopLeft);
    entries->setSfFontSize(24);
    entries->setSfFont(hud->journalFont);
    entries->normalColor = sf::Color::White;
    addChild(entries);
}

void Inventory::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    HudMenu::update(window, gameTime);

    LevelClient* levelClient = hud->state->levelClient;
    if(levelClient->playerCharacter)
    {
        std::string entry("Inventory\n");
        for(auto iterator = levelClient->playerCharacter->backpackClient.itemClients.begin(); iterator != levelClient->playerCharacter->backpackClient.itemClients.end(); iterator++)
        {
            // Level 1
            ItemClient* i = &iterator->second;
            entry += "- " + Item::getTitle(i->schema.type) + "\n";
        }
        entries->setSfFontString(entry);
        entries->opacity = opacity;
    }
}