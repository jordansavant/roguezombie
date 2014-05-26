#include "Tooltip.hpp"
#include "Hud.hpp"
#include "Frame.hpp"
#include "../../bitengine/Input.hpp"
#include "../StateGamePlay.hpp"
#include <sstream>
#include <iostream>
#include <iterator>

Tooltip::Tooltip(Hud* _hud)
    : Frame(_hud, 0, 0, 300, 80, bit::Element::AnchorType::TopLeft), isActive(false), fadeTimer(3)
{
    useBottomPointer = true;
    managesOpacity = true;
    opacity = 0;
    color = sf::Color(123, 123, 123);

    information = new bit::Label(15, 10, 0, 0, bit::Element::AnchorType::TopLeft);
    information->setSfFontSize(24);
    information->setSfFont(hud->journalFont);
    information->normalColor = color;
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

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

void Tooltip::displayAt(std::string &info, int screenX, int screenY)
{
    // change tooltip size based on occurences
    int w = 1;
    std::vector<std::string> tokens = split(info, '\n');
    int n = tokens.size();
    for(unsigned int i=0; i < n; i++)
    {
        int wc = tokens[i].size();
        w = std::max(w, wc);
    }

    information->setSfFontString(info);
    targetHeight = 30 + information->fontSize * n;
    targetWidth = 50 + (int)((float)(information->fontSize * w) / 2.1f);
    screenY -= targetHeight * elementScale + Frame::bottomPointerSprite->height;
    screenX -= targetWidth * elementScale / 2;
    relativePosition.x = screenX / elementScale;
    relativePosition.y = screenY / elementScale;
    activate();
}