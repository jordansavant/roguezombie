#include "StateMenu.hpp"
#include <functional>
#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Input.hpp"
#include "../RogueZombieGame.hpp"
#include "../../ResourcePath.h"

StateMenu::StateMenu(RogueZombieGame* rogueZombieGame)
    : bit::Container(0, 0, rogueZombieGame->targetResolution.x, rogueZombieGame->targetResolution.y, bit::Element::AnchorType::Center), rogueZombieGame(rogueZombieGame)
{
    // Colors
    rzRed = sf::Color(216, 30, 30);

    // Assets
    std::string titleImagePath(resourcePath() + "minititle.png");
    titleTexture.loadFromFile(titleImagePath);
    titleTexture.setSmooth(true);

    std::string fontPath(resourcePath() + "homespun.ttf");
    menuFont.loadFromFile(fontPath);
    menuFontSize = 48;

    lambdaListenToInput = [] (bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
    {
        bit::Container* container = static_cast<bit::Container*>(element);
        sf::Vector2i pixelPos = sf::Mouse::getPosition(*window);

        bool in = false;
        for(int i = 0; i < container->childElements.size(); i++)
        {
            if(container->childElements[i]->canHaveFocus)
            {
                if(container->childElements[i]->contains(pixelPos.x, pixelPos.y))
                {
                    in = true;
                    container->changeFocusedChild(i);
                    break;
                }
            }
        }

        if(!in)
        {
            container->clearFocusedChild();
        }

        // Never block child input
        return false;
    };
    canHaveFocus = true;
    isInfocus = true;
}

void StateMenu::load()
{
}

void StateMenu::configureLabel(bit::Label* label, std::string text)
{
    label->canHaveFocus = true;
    label->setSfFont(menuFont);
    label->setSfFontSize(menuFontSize);
    label->setSfFontString(text);
    label->normalColor = sf::Color::White;
    label->focusedColor = rzRed;
    label->paddingTop = 0;
    label->paddingLeft = 5;
    label->paddingRight = 5;
    label->paddingBottom = 25;
    label->opacity = 0;
    addChild(label);
}

void StateMenu::inflowLabel(bit::Element* element, float startingX, float startingY, float delay, float endX, float endY)
{
    element->relativePosition.x = startingX;
    element->relativePosition.y = startingY;
    element->opacity = 0;
    element->queueEffect(new bit::MoveEffect(delay, endX, endY))->queueEffect(new bit::FadeEffect(500, 1));
}

void StateMenu::outflowLabel(bit::Element* element, float delay, float endX, float endY)
{
    element->queueEffect(new bit::Effect(delay))->queueEffect(new bit::FadeEffect(500, 0));
}