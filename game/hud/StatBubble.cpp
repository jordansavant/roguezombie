#include "StatBubble.hpp"
#include "Hud.hpp"
#include "Frame.hpp"
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
#include <sstream>

StatBubble::StatBubble(Hud* _hud)
    : Frame(_hud, 50, 0, 200, 150, bit::Element::AnchorType::Left), isActive(false), tileId(0), tileClient(NULL), refreshTimer(2), fadeTimer(3)
{
    useBottomPointer = true;
    managesOpacity = true;
    opacity = 0;
}

void StatBubble::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(tileClient)
    {
        sf::Vector2i mapping = window.mapCoordsToPixel(sf::Vector2f(tileClient->centerRenderX, tileClient->centerRenderY));
        left = mapping.x - width / 2;
        top = mapping.y - height - tileClient->height;
        Frame::updateReals(window, gameTime);

        if(isActive && refreshTimer.update(gameTime))
        {
            hud->state->requestStatisticsForBodyOnTile(tileId);
        }
        if(isActive && fadeTimer.update(gameTime))
        {
            deactivate();
        }
    }
}

void StatBubble::activate()
{
    if(isActive == false)
    {
        fadeTimer.reset();
    }

    isActive = true;
    show();
}

void StatBubble::deactivate()
{
    tileId = 0;
    tileClient = NULL;
    isActive = false;
    clearChildren();

    hide();
}

void StatBubble::hide()
{
    canHaveFocus = false;
    clearEffects();
    immediateEffect(new bit::FadeEffect(150, 0, bit::Easing::Linear, [](bit::Element* e, bit::Effect* f){
        bool crap = true;
    }));
}

void StatBubble::show()
{
    canHaveFocus = true;
    clearEffects();
    immediateEffect(new bit::FadeEffect(150, 1));
}

void StatBubble::handleStats(bit::ServerPacket &packet, unsigned int tileId)
{
    clearChildren();
    activate();

    this->tileId = tileId;
    this->tileClient = hud->state->levelClient->tiles[tileId];

    int y = 10;
    int x = 15;
    
    // unpack info
    Character::Type type;
    int health, maxHealth, speed;
    bit::NetworkHelper::unpackEnum<sf::Uint32, Character::Type>(packet, type);
    packet >> health >> maxHealth >> speed;

    // Title
    bit::Label* label = new bit::Label(x, y, 0, 0, bit::Element::AnchorType::TopLeft);
    label->setSfFontSize(24);
    label->setSfFont(hud->journalFont);
    label->normalColor = sf::Color::White;
    label->setSfFontString(Character::getTitle(type));
    label->paddingRight = 10;
    label->paddingBottom = 10;
    addChild(label);
    y += 30;

    // Health
    std::stringstream ssa;
    ssa << health << "/" << maxHealth << " HP";
    label = new bit::Label(x, y, 0, 0, bit::Element::AnchorType::TopLeft);
    label->setSfFontSize(24);
    label->setSfFont(hud->journalFont);
    label->normalColor = sf::Color::White;
    label->setSfFontString(ssa.str());
    label->paddingRight = 10;
    label->paddingBottom = 10;
    addChild(label);
    y += 30;
    
    // Speed
    std::stringstream ssb;
    ssb << speed << " Speed";
    label = new bit::Label(x, y, 0, 0, bit::Element::AnchorType::TopLeft);
    label->setSfFontSize(24);
    label->setSfFont(hud->journalFont);
    label->normalColor = sf::Color::White;
    label->setSfFontString(ssb.str());
    label->paddingRight = 10;
    label->paddingBottom = 10;
    addChild(label);
    y += 30;

}