#include "Tooltip.hpp"
#include "Hud.hpp"
#include "Frame.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/System.hpp"
#include "../StateGamePlay.hpp"
#include <sstream>
#include <iostream>
#include <iterator>

Tooltip::Tooltip(Hud* _hud)
    : Frame(_hud, 0, 0, 300, 80, bit::Element::AnchorType::TopLeft), isActive(false), fadeTimer(3)
{
    useBottomPointer = true;
    managesOpacity = true;
    opacity = requestedOpacity = 0;
    requestedDelay = 750;
    requestedDuration = 3000;
    color = sf::Color(0, 255, 0);
    Frame::z = Hud::getDrawDepth(1);
    scaleStyle = ScaleStyle::PowerOfTwo;

    information = new bit::Label(0, 10, 0, 0, bit::Element::AnchorType::TopLeft);
    information->scaleStyle = ScaleStyle::PowerOfTwo;
    information->setSfFontSize(Hud::font_noteSize);
    information->setSfFont(hud->journalFont);
    information->normalColor = color;
    information->setSfFontString(std::string(""));
    information->paddingRight = 0;
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
    opacity = 0;
}

void Tooltip::show()
{
    canHaveFocus = true;
    clearEffects();
    queueEffect(new bit::Effect(requestedDelay))->queueEffect(new bit::FadeEffect(0, requestedOpacity, bit::Easing::Instant));
}

void Tooltip::displayAt(std::string &info, int screenX, int screenY, float opacity, int delay, int duration, int width, int height)
{
    int wPad = 30;
    int hPad = 25;
    this->requestedOpacity = opacity;
    this->requestedDelay = delay;
    this->requestedDuration = duration;

    fadeTimer.setNewDuration((float)duration / 1000.0f);
    information->setSfFontString(info);
    
    targetWidth = width < 0 ? information->targetWidth + wPad : width;
    targetHeight = height < 0 ? information->targetHeight + hPad : height;
    information->relativePosition.x = (targetWidth - information->targetWidth) / 2;

    screenY -= targetHeight * elementScale + Frame::bottomPointerSprite->height;
    screenX -= targetWidth * elementScale / 2;
    relativePosition.x = screenX / elementScale;
    relativePosition.y = screenY / elementScale;

    activate();
}