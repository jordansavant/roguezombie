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
    immediateEffect(new bit::FadeEffect(0, 0, bit::Easing::Instant));
}

void Tooltip::show()
{
    canHaveFocus = true;
    clearEffects();
    queueEffect(new bit::Effect(750))->queueEffect(new bit::FadeEffect(0, requestedOpacity, bit::Easing::Instant));
}

void Tooltip::displayAt(std::string &info, int screenX, int screenY, float opacity, int width, int height)
{
    int wPad = 30;
    int hPad = 25;
    int ttWidth = 0;
    int ttHeight = 0;
    this->requestedOpacity = opacity;

    // change tooltip size based on occurences of characters
    if(width < 0 || height < 0)
    {
        float targetCharWidth = .50f;
        int w = 1;
        std::vector<std::string> tokens = bit::String::split(info, '\n');
        int n = tokens.size();
        for(unsigned int i=0; i < n; i++)
        {
            int wc = tokens[i].size();
            w = std::max(w, wc);
        }
        ttHeight = hPad + information->fontSize * n;
        ttWidth = wPad + (int)((float)(information->fontSize * w * targetCharWidth));
    }
    else
    {
        ttWidth = width;
        ttHeight = height;
    }

    information->setSfFontString(info);
    
    targetWidth = information->targetWidth + wPad;
    targetHeight = information->targetHeight + hPad;
    information->relativePosition.x = (targetWidth - information->targetWidth) / 2;

    screenY -= targetHeight * elementScale + Frame::bottomPointerSprite->height;
    screenX -= targetWidth * elementScale / 2;
    relativePosition.x = screenX / elementScale;
    relativePosition.y = screenY / elementScale;
    activate();
}