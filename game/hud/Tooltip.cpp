#include "Tooltip.hpp"
#include "Hud.hpp"
#include "Frame.hpp"
#include "../../bitengine/Input.hpp"
#include "../StateGamePlay.hpp"
#include <sstream>

Tooltip::Tooltip(Hud* _hud)
    : Frame(_hud, 0, 0, 300, 80, bit::Element::AnchorType::TopLeft), isActive(false), fadeTimer(3)
{
    useBottomPointer = true;
    managesOpacity = true;
    opacity = 0;

    information = new bit::Label(15, 10, 0, 0, bit::Element::AnchorType::TopLeft);
    information->setSfFontSize(24);
    information->setSfFont(hud->journalFont);
    information->normalColor = sf::Color::White;
    information->setSfFontString(std::string(""));
    information->paddingRight = 10;
    information->paddingBottom = 10;
    addChild(information);
}

void Tooltip::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    Frame::updateReals(window, gameTime);

    if(isActive && fadeTimer.update(gameTime))
    {
        deactivate();
    }
}

void Tooltip::activate()
{
    fadeTimer.reset();

    isActive = true;
    show();
}

void Tooltip::deactivate()
{
    isActive = false;
    hide();
}

void Tooltip::hide()
{
    canHaveFocus = false;
    clearEffects();
    immediateEffect(new bit::FadeEffect(150, 0, bit::Easing::Linear));
}

void Tooltip::show()
{
    canHaveFocus = true;
    clearEffects();
    immediateEffect(new bit::FadeEffect(150, 1));
}

void Tooltip::displayAt(std::string &info, int screenX, int screenY)
{
    // TODO: accept that their X and Y request is at our pointer tip
    screenY -= height + Frame::bottomPointerSprite->height;
    screenX -= width / 2;

    information->setSfFontString(info);
    relativePosition.x = screenX / elementScale;
    relativePosition.y = screenY / elementScale;
    activate();
}