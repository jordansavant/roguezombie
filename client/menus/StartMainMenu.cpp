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
    unsigned int fSize = 32;

    centroid = new bit::Container(0, 0, 800, 600, bit::Element::AnchorType::Center);
    centroid->scaleStyle = ScaleStyle::None;
    centroid->canHaveFocus = true;
    centroid->isInfocus = true;
    centroid->lambdaListenToInput = lambdaListenToInput;
    addChild(centroid);

    // Title
    titlePicture = new bit::Picture(0, 0, 0, 0, bit::Element::AnchorType::Center);
    //titlePicture->opacity = 0;
    titlePicture->setTexture(titleTexture);
    titlePicture->setColor(sf::Color::White);
    titlePicture->scaleStyle = ScaleStyle::None;
    addChild(titlePicture);

    // Singleplayer
    singleplayerLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::TopRight,
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
    configureLabel(singleplayerLabel, "New Game");
    singleplayerLabel->normalColor = rzGreen;
    singleplayerLabel->scaleStyle = ScaleStyle::None;
    singleplayerLabel->setSfFontSize(fSize);
    singleplayerLabel->makeHoverable(rogueZombieGame->inputManager,
        [] (bit::Hoverable* h, bit::Element* e) {
            e->queueEffect(new bit::MoveEffect(100, -6, 0));
        },
        [] (bit::Hoverable* h, bit::Element* e) {
            e->queueEffect(new bit::MoveEffect(100, 6, 0));
        }
    );
    centroid->addChild(singleplayerLabel);

    // Multiplayer
    multiplayerLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::TopRight,
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
    configureLabel(multiplayerLabel, "Join Game");
    multiplayerLabel->normalColor = rzGreen;
    multiplayerLabel->scaleStyle = ScaleStyle::None;
    multiplayerLabel->setSfFontSize(fSize);
    multiplayerLabel->makeHoverable(rogueZombieGame->inputManager,
        [] (bit::Hoverable* h, bit::Element* e) {
            e->queueEffect(new bit::MoveEffect(100, -6, 0));
        },
        [] (bit::Hoverable* h, bit::Element* e) {
            e->queueEffect(new bit::MoveEffect(100, 6, 0));
        }
    );
    centroid->addChild(multiplayerLabel);

    // Options
    settingsLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::BottomLeft, std::bind(&StartMainMenu::onActivate_Settings, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    configureLabel(settingsLabel, "Configure");
    settingsLabel->normalColor = rzGreen;
    settingsLabel->scaleStyle = ScaleStyle::None;
    settingsLabel->setSfFontSize(fSize);
    settingsLabel->makeHoverable(rogueZombieGame->inputManager,
        [] (bit::Hoverable* h, bit::Element* e) {
            e->queueEffect(new bit::MoveEffect(100, 6, 0));
        },
        [] (bit::Hoverable* h, bit::Element* e) {
            e->queueEffect(new bit::MoveEffect(100, -6, 0));
        }
    );
    centroid->addChild(settingsLabel);

    // Exit
    exitLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::BottomLeft,
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
    exitLabel->normalColor = rzGreen;
    exitLabel->scaleStyle = ScaleStyle::None;
    exitLabel->setSfFontSize(fSize);
    exitLabel->makeHoverable(rogueZombieGame->inputManager,
        [] (bit::Hoverable* h, bit::Element* e) {
            e->queueEffect(new bit::MoveEffect(100, 6, 0));
        },
        [] (bit::Hoverable* h, bit::Element* e) {
            e->queueEffect(new bit::MoveEffect(100, -6, 0));
        }
    );
    centroid->addChild(exitLabel);
}

void StartMainMenu::load()
{
    int titleFadeDelay = 1000;
    int titleFadeTime = 500;
    int titleMoveDelay = 500;
    int titleMoveTime = 1000;
    //titlePicture->queueEffect(new bit::Effect(titleFadeDelay))->queueEffect(new bit::FadeEffect(titleFadeTime, 1));
    //titlePicture->queueEffect(new bit::Effect(titleMoveDelay))->queueEffect(new bit::MoveEffect(titleMoveTime, bit::Easing::Type::InOutQuint, 0, -240));

    showMainMenu(2000);
}

void StartMainMenu::showMainMenu(int additionalDelay)
{
    int fx = -510;
    int fy = 40;
    int yy = 10;
    int delay = 0 + additionalDelay;
    inflowLabel(singleplayerLabel, 500, yy, delay + 800, fx, 0);
    inflowLabel(multiplayerLabel, 500, yy + fy,  delay + 700, fx, 0);
    inflowLabel(settingsLabel, -500, -yy - fy,  delay + 600, -fx, 0);
    inflowLabel(exitLabel, -500, -yy,  delay + 500, -fx, 0);
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