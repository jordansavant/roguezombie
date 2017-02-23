#include "Journal.hpp"
#include "Hud.hpp"
#include "../../bitengine/Input.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"

Journal::Journal(Hud* _hud)
    : HudMenu(_hud, 0, 0, 300, 720, bit::Element::AnchorType::Right)
{
    journalEntries = new bit::Label(10, 10, 0, 0, bit::Element::AnchorType::TopLeft);
    journalEntries->setSfFontSize(24);
    journalEntries->setSfFont(hud->journalFont);
    journalEntries->normalColor = sf::Color::White;
    addChild(journalEntries);
}

void Journal::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    HudMenu::update(window, gameTime);

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
        journalEntries->setSfFontString(entry);
        journalEntries->opacity = opacity;
    }
}

void Journal::hide()
{
    isShown = false;
    clearEffects();
    relativePosition.x = 0;
    immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, targetWidth, 0));
    immediateEffect(new bit::FadeEffect(150, 0));
}

void Journal::show()
{
    isShown = true;
    clearEffects();
    relativePosition.x = targetWidth;
    immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, -targetWidth, 0));
    immediateEffect(new bit::FadeEffect(300, 1));
}