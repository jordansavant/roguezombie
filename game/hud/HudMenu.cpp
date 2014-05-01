#include "HudMenu.hpp"
#include "Hud.hpp"
#include "../../bitengine/Input.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"

HudMenu::HudMenu(Hud* hud)
    : bit::Container(-50, 50, 300, 500, bit::Element::AnchorType::TopRight, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), hud(hud), isShown(false)
{
}

HudMenu::HudMenu(Hud* hud, float relativeX, float relativeY, float width, float height, AnchorType anchorType)
    : bit::Container(relativeX, relativeY, width, height, anchorType, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), hud(hud), isShown(false)
{
}

HudMenu::HudMenu(Hud* hud, float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput)
    : bit::Container(relativeX, relativeY, width, height, anchorType, lambdaListenToInput), hud(hud), isShown(false)
{
}

void HudMenu::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::Container::update(window, gameTime);
}

void HudMenu::hide()
{
    canHaveFocus = false;
    opacity = 0;
    isShown = false;
}

void HudMenu::show()
{
    canHaveFocus = true;
    opacity = 1;
    isShown = true;
}

void HudMenu::toggle()
{
    isShown ? hide() : show();
}