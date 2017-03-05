#include "StartMainMenu.hpp"
#include <functional>
#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Input.hpp"
#include "../RogueZombieGame.hpp"
#include "../StateGameStart.hpp"
#include "StateMenu.hpp"

StartMainMenu::StartMainMenu(RogueZombieGame* rogueZombieGame, StateGameStart* state)
    : StateMenu(rogueZombieGame), state(state)
{
    // Title
    titlePicture = new bit::Picture(0, 0, 0, 0, bit::Element::AnchorType::Center);
    titlePicture->opacity = 0;
    titlePicture->setTexture(titleTexture);
    titlePicture->setColor(sf::Color::White);
    addChild(titlePicture);

    // Singleplayer
    singleplayerLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::Center,
        [rogueZombieGame, state] (Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
        {
            if(element->opacity == 1 && rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left))
            {
                state->requestStackPush(RogueZombieGame::stateGamePlayHost);
                return true;
            }

            return false;
        }
    );
    configureLabel(singleplayerLabel, "Host Game");

    // Multiplayer
    multiplayerLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::Center,
        [rogueZombieGame, state] (Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
        {
            if(element->opacity == 1 && rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left))
            {
                state->requestStackPush(RogueZombieGame::stateGamePlayClient);
                return true;
            }

            return false;
        }
    );
    configureLabel(multiplayerLabel, "Join Server");

    // Options
    settingsLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::Center, std::bind(&StartMainMenu::onActivate_Settings, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    configureLabel(settingsLabel, "Configure");

    // Exit
    exitLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::Center,
        [rogueZombieGame, state] (Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
        {
            if(element->opacity == 1 && rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left))
            {
                state->requestStackPop();
                return true;
            }

            return false;
        }
    );
    configureLabel(exitLabel, "Exit");
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
    outflowLabel(singleplayerLabel, delay + 300, 0, 0);
    outflowLabel(multiplayerLabel, delay + 200, 0, 0);
    outflowLabel(settingsLabel, delay + 100, 0, 0);
    outflowLabel(exitLabel, delay + 0, 0, 0);
}

bool StartMainMenu::onActivate_Settings(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime)
{
    if(rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left))
    {
        hideMainMenu();
        return true;
    }

    return false;
}