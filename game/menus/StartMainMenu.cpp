#include "StartMainMenu.hpp"
#include <functional>
#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Input.hpp"
#include "../RogueZombieGame.hpp"
#include "../StateGameStart.hpp"

StartMainMenu::StartMainMenu(RogueZombieGame* rogueZombieGame, StateGameStart* state)
    : bit::Container(0, 0, rogueZombieGame->targetResolution.x, rogueZombieGame->targetResolution.y, bit::Element::AnchorType::Center), rzGame(rogueZombieGame), state(state)
{
    // Colors
    rzRed = sf::Color(216, 30, 30);


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

    // Title
    titlePicture = new bit::Picture(0, 0, 0, 0, bit::Element::AnchorType::Center);
    titlePicture->opacity = 0;
    titlePicture->setTexture(state->titleTexture);
	titlePicture->setColor(sf::Color::White);
    addChild(titlePicture);

    // Singleplayer
    singleplayerLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::Center,
        [rogueZombieGame, state] (Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
        {
            if(rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left))
            {
                state->requestStateClear();
                state->requestStackPush(RogueZombieGame::stateGamePlayHost);
                return true;
            }

            return false;
        }
    );
    configureLabel(singleplayerLabel, "singleplayer");

    // Multiplayer
    multiplayerLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::Center,
        [rogueZombieGame, state] (Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
        {
            if(rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left))
            {
                state->requestStateClear();
                state->requestStackPush(RogueZombieGame::stateGamePlayClient);
                return true;
            }

            return false;
        }
    );
    configureLabel(multiplayerLabel, "multiplayer");

    // Options
    settingsLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::Center, std::bind(&StartMainMenu::onActivate_Settings, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    configureLabel(settingsLabel, "settings");

    // Exit
    exitLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::Center,
        [rogueZombieGame, state] (Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
        {
            if(rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left))
            {
                state->requestStateClear();
                return true;
            }

            return false;
        }
    );
    configureLabel(exitLabel, "exit");
}



void StartMainMenu::load()
{
    int titleFadeDelay = 1000;
    int titleFadeTime = 1000;
    int titleMoveDelay = 500;
    int titleMoveTime = 1000;
    titlePicture->queueEffect(new bit::Effect(titleFadeDelay))->queueEffect(new bit::FadeEffect(titleFadeTime, 1));
    titlePicture->queueEffect(new bit::Effect(titleMoveDelay))->queueEffect(new bit::MoveEffect(titleMoveTime, bit::Easing::Type::InOutQuint, 0, -240));

    showMainMenu(2000);
}



void StartMainMenu::showMainMenu(int additionalDelay)
{
    int delay = 0 + additionalDelay;
    inflowLabel(singleplayerLabel, 0, -600, delay + 1100, 0, 520);
    inflowLabel(multiplayerLabel, 0, -600,  delay + 1000, 0, 580);
    inflowLabel(settingsLabel, 0, -600,  delay + 900, 0, 640);
    inflowLabel(exitLabel, 0, -600,  delay + 800, 0, 700);
}

void StartMainMenu::hideMainMenu(int additionalDelay)
{
    int delay = 0 + additionalDelay;
    outflowLabel(singleplayerLabel, delay + 300);
    outflowLabel(multiplayerLabel, delay + 200);
    outflowLabel(settingsLabel, delay + 100);
    outflowLabel(exitLabel, delay + 0);
}




void StartMainMenu::configureLabel(bit::Label* label, std::string text)
{
    label->canHaveFocus = true;
    label->setFont(state->menuFont);
    label->setCharacterSize(state->menuFontSize);
	label->setString(text);
	label->normalColor = sf::Color::White;
    label->focusedColor = rzRed;
    label->paddingTop = 0;
    label->paddingLeft = 5;
    label->paddingRight = 5;
    label->paddingBottom = 25;
    label->opacity = 0;
    addChild(label);
}

void StartMainMenu::inflowLabel(bit::Element* element, float startingX, float startingY, float delay, float endX, float endY)
{
    element->relativePosition.x = startingX;
    element->relativePosition.y = startingY;
    element->opacity = 0;
    element->queueEffect(new bit::MoveEffect(delay, endX, endY))->queueEffect(new bit::FadeEffect(500, 1));
}

void StartMainMenu::outflowLabel(bit::Element* element, float delay)
{
    element->queueEffect(new bit::Effect(delay))->queueEffect(new bit::FadeEffect(500, 0));
}



bool StartMainMenu::onActivate_Settings(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime)
{
    if(rzGame->inputManager->isButtonReleased(sf::Mouse::Left))
    {
        hideMainMenu();
        return true;
    }

    return false;
}