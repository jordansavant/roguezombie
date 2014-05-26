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
    : Frame(_hud, 50, 0, 150, 115, bit::Element::AnchorType::Left), isActive(false), tileId(0), tileClient(NULL), refreshTimer(.1), fadeTimer(3)
{
    useBottomPointer = true;
    managesOpacity = true;
    opacity = 0;
}

void StatBubble::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(tileClient && tileClient->hasCharacter && !tileClient->characterClient->schema.isDead())
    {
        int x = tileClient->characterClient->sprite.screenX;
        int y = tileClient->characterClient->sprite.screenY;
        sf::Vector2i mapping = window.mapCoordsToPixel(sf::Vector2f(x, y));
        left = mapping.x - width / 2;
        top = mapping.y - height - tileClient->height * 1.25;
        Frame::updateReals(window, gameTime);

        if(isActive && refreshTimer.update(gameTime))
        {
            handleStats(tileId);
        }
    }
    else if(isActive)
    {
        deactivate();
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
    immediateEffect(new bit::FadeEffect(150, 0, bit::Easing::Linear));
}

void StatBubble::show()
{
    canHaveFocus = true;
    clearEffects();
    immediateEffect(new bit::FadeEffect(150, 1));
}

void StatBubble::handleStats(unsigned int tileId)
{
    clearChildren();
    activate();

    auto itr = hud->state->levelClient->tiles.find(tileId);
    if(itr == hud->state->levelClient->tiles.end())
    {
        deactivate();
        return;
    }

    this->tileId = tileId;
    this->tileClient = itr->second;
    Hud* hudx = hud;

    int y = 10;
    int x = 17;
    
    // Get info
    Character::Type type = tileClient->characterClient->schema.type;
    int health = tileClient->characterClient->schema.health;
    int maxHealth = tileClient->characterClient->schema.maxHealth;
    int speed = tileClient->characterClient->schema.speed;

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
    ssa << "HP " << health << "/" << maxHealth;
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
    ssb << "Speed " << speed;
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