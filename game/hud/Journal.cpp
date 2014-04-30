#include "Journal.hpp"
#include "Hud.hpp"
#include "../../bitengine/Input.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"

Journal::Journal(Hud* _hud)
    : bit::Container(0, 50, 500, 200, bit::Element::AnchorType::TopRight), hud(_hud)
{
    journalEntries = new bit::Label(0, 0, 0, 0, bit::Element::AnchorType::TopLeft);
    journalEntries->setSfFontSize(24);
    journalEntries->setSfFont(hud->journalFont);
    journalEntries->normalColor = sf::Color::White;
    addChild(journalEntries);
}

void Journal::updateJournal(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::Container::update(window, gameTime);

    LevelClient* levelClient = hud->state->levelClient;

    if(levelClient->playerCharacter)
    {
        std::string entry("Journal\n");
        for(auto iterator = levelClient->playerCharacter->missionClients.begin(); iterator != levelClient->playerCharacter->missionClients.end(); iterator++)
        {
            // Level 1
            MissionClient* m = &iterator->second;
            if(m->schema.isComplete)
                entry += "- " + JournalEntry::getTitle(m->schema.journalEntry) + " - Complete\n";
            else
                entry += "- " + JournalEntry::getTitle(m->schema.journalEntry) + "\n";

            // Level 2
            for(auto iterator2 = iterator->second.childMissions.begin(); iterator2 != iterator->second.childMissions.end(); iterator2++)
            {
                MissionClient* mc = &iterator2->second;
                if(mc->schema.isComplete)
                    entry += "  - " + JournalEntry::getTitle(mc->schema.journalEntry) + " - Complete\n";
                else
                    entry += "  - " + JournalEntry::getTitle(mc->schema.journalEntry) + "\n";
            }
        }

        entry += "\n\nItems\n";
        for(auto iterator = levelClient->playerCharacter->backpackClient.itemClients.begin(); iterator != levelClient->playerCharacter->backpackClient.itemClients.end(); iterator++)
        {
            // Level 1
            ItemClient* i = &iterator->second;
            entry += "- " + Item::getTitle(i->schema.type) + "\n";
        }

        journalEntries->setSfFontString(entry);
    }
}