#include "Journal.hpp"
#include "Hud.hpp"
#include "../../bitengine/Input.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"

Journal::Journal(Hud* _hud)
    : HudMenu(_hud, 0, 0, 691, 728, bit::Element::AnchorType::Right, true)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    managesOpacity = true;

    journalEntries = new bit::Label(38, 30, 0, 0, bit::Element::AnchorType::TopLeft);
    journalEntries->setSfFontSize(Hud::font_primarySize);
    journalEntries->setSfFont(hud->journalFont);
    journalEntries->normalColor = sf::Color::Black;
    journalEntries->scaleStyle = ScaleStyle::PowerOfTwo;
    addChild(journalEntries);

    // Icon
    naviconSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("JournalNavicon");
    naviconQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
}

void Journal::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    HudMenu::update(window, gameTime);

    LevelClient* levelClient = hud->state->levelClient;
    if(levelClient->playerCharacter)
    {
        // Flub divider (text does not scale perfectly)
        std::string entry;
        if(elementScale == 1)
            entry = std::string("OBJECTIVES:\n-----------------------------------------\n\n");
        else
            entry = std::string("OBJECTIVES:\n--------------------------------------------\n\n");

        for(auto iterator = levelClient->playerCharacter->missionClients.begin(); iterator != levelClient->playerCharacter->missionClients.end(); iterator++)
        {
            // Level 1
            MissionClient* m = &iterator->second;
            if(m->schema.isComplete)
                entry += JournalEntry::getTitle(m->schema.journalEntry) + " - Complete\n";
            else
                entry += JournalEntry::getTitle(m->schema.journalEntry) + "\n";

            // Level 2
            for(auto iterator2 = iterator->second.childMissions.begin(); iterator2 != iterator->second.childMissions.end(); iterator2++)
            {
                MissionClient* mc = &iterator2->second;
                if(mc->schema.isComplete)
                    entry += "-> " + JournalEntry::getTitle(mc->schema.journalEntry) + " // complete\n";
                else
                    entry += "-> " + JournalEntry::getTitle(mc->schema.journalEntry) + "\n";
            }
        }
        journalEntries->setSfFontString(entry);
        journalEntries->opacity = opacity;
    }
}

void Journal::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    HudMenu::updateTargets(window, gameTime);

    // Navicon
    bit::Vertex3* naviconQuad = &hud->interfaceVertexMap.vertexArray[naviconQuadIndex];
    bit::VertexHelper::colorQuad(naviconQuad, sf::Color(255, 255, 255, opacity * 255));
}

void Journal::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    HudMenu::updateReals(window, gameTime);

    // Navicon
    float naviconZ = Hud::getDrawDepth(Hud::zindex_frames);
    bit::Vertex3* naviconQuad = &hud->interfaceVertexMap.vertexArray[naviconQuadIndex];
    naviconSprite->applyToQuad(naviconQuad);
    bit::VertexHelper::positionQuad(naviconQuad, (int)left, (int)(top - naviconSprite->height * elementScale), naviconZ, naviconSprite->width, naviconSprite->height, elementScale);
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
    immediateEffect(new bit::FadeEffect(300, Hud::popupOpacity));
}