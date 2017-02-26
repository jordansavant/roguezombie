#include "HudMenu.hpp"
#include "Hud.hpp"
#include "../../bitengine/Input.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"

HudMenu::HudMenu(Hud* hud)
    : Frame(hud, 350, 50, 300, 500, bit::Element::AnchorType::TopRight, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3), false), isShown(false)
{
    originX = relativePosition.x;
    originY = relativePosition.y;
}

HudMenu::HudMenu(Hud* hud, float relativeX, float relativeY, float width, float height, AnchorType anchorType, bool bgGreen)
    : Frame(hud, relativeX, relativeY, width, height, anchorType, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3), bgGreen), isShown(false)
{
    originX = relativePosition.x;
    originY = relativePosition.y;
}

HudMenu::HudMenu(Hud* hud, float relativeX, float relativeY, float width, float height, AnchorType anchorType, std::function<bool(Element*, sf::RenderWindow*, sf::Time*)> lambdaListenToInput, bool bgGreen)
    : Frame(hud, relativeX, relativeY, width, height, anchorType, lambdaListenToInput, bgGreen), isShown(false)
{
    originX = relativePosition.x;
    originY = relativePosition.y;
}

void HudMenu::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    Frame::update(window, gameTime);
}

void HudMenu::hide()
{
    canHaveFocus = false;
    isShown = false;
    clearEffects();
    immediateEffect(new bit::FadeEffect(150, 0));
}

void HudMenu::show()
{
    canHaveFocus = true;
    isShown = true;
    clearEffects();
    relativePosition.x = originX;
    immediateEffect(new bit::FadeEffect(300, 1));
}

void HudMenu::toggle()
{
    isShown ? hide() : show();
}