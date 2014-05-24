#include "ActionBar.hpp"
#include "Hud.hpp"
#include "HudElement.hpp"

ActionBar::ActionBar(Hud* hud)
    : Frame(hud, 0, 200, 1100, 200, bit::Element::AnchorType::Bottom, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3))
{
    scaleStyle = bit::Element::ScaleStyle::PowerOfTwo;

    int originX = 20;
    int originY = 10;
    int xpadding = 2;

    attack = new HudElement(originX, originY, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    attack->load(hud, std::string("optionbar_options"));
    attack->onActivate = [hud] (Element* e) {
    };
    addChild(attack);
    originX += attack->sprite->width + xpadding;

    move = new HudElement(originX, originY, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    move->load(hud, std::string("optionbar_character"));
    move->onActivate = [hud] (Element* e) {
    };
    addChild(move);
    originX += attack->sprite->width + xpadding;
}

void ActionBar::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    Frame::update(window, gameTime);
}

void ActionBar::hide()
{
    canHaveFocus = false;
    clearEffects();
    relativePosition.y = 120;
    immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, 0, targetHeight - 120));
    immediateEffect(new bit::FadeEffect(150, 0));
}

void ActionBar::show()
{
    canHaveFocus = true;
    clearEffects();
    relativePosition.y = targetHeight;
    immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, 0, -targetHeight + 120));
    immediateEffect(new bit::FadeEffect(300, 1));
}
