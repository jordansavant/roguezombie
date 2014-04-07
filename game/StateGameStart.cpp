#include "StateGameStart.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Input.hpp"
#include "../ResourcePath.h"
#include "RogueZombieGame.hpp"

StateGameStart::StateGameStart(bit::StateStack &stack, RogueZombieGame* _game)
    : bit::State(stack, _game), rogueZombieGame(_game)
{
    createCamera(rogueZombieGame, 0, 0, 1, 1);

    std::string titleImagePath(resourcePath() + "minititle.png");
    titleTexture.loadFromFile(titleImagePath);
    titleTexture.setSmooth(true);

    std::string fontPath(resourcePath() + "homespun.ttf");
    font.loadFromFile(fontPath);
    int fontsize = 48;

    mainMenu = new bit::Container(0, 0, rogueZombieGame->targetResolution.x, rogueZombieGame->targetResolution.y, bit::Element::AnchorType::Center);

    // Title image
    titlePicture = new bit::Picture(0, 0, 0, 0, bit::Element::AnchorType::Center);
    titlePicture->opacity = 0;
    titlePicture->setTexture(titleTexture);
	titlePicture->setColor(sf::Color::White);
    titlePicture->queueEffect(new bit::Effect(1000))->queueEffect(new bit::FadeEffect(1000, 1));

    titlePicture->queueEffect(new bit::MoveEffect(1000, bit::Easing::Type::InBack, 0, -120));
    titlePicture->queueEffect(new bit::MoveEffect(1000, bit::Easing::Type::OutBack, 0, -120));
    titlePicture->queueEffect(new bit::MoveEffect(2000, bit::Easing::Type::InOutBack, 0, 240));
    titlePicture->queueEffect(new bit::MoveEffect(2000, bit::Easing::Type::InOutBack, 0, -240));

    mainMenu->addChild(titlePicture);

    // Singleplayer
    singleplayerLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::Center);
    singleplayerLabel->canHaveFocus = true;
    singleplayerLabel->setFont(font);
	singleplayerLabel->setCharacterSize(fontsize);
	singleplayerLabel->setString("singleplayer");
    singleplayerLabel->normalColor = sf::Color::White;
    singleplayerLabel->focusedColor = sf::Color::Yellow;
    singleplayerLabel->paddingTop = -2;
    singleplayerLabel->paddingLeft = 5;
    singleplayerLabel->paddingRight = 5;
    singleplayerLabel->paddingBottom = 6;
    singleplayerLabel->opacity = 0;
    mainMenu->addChild(singleplayerLabel);

    // Multiplayer
    multiplayerLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::Center);
    multiplayerLabel->canHaveFocus = true;
    multiplayerLabel->setFont(font);
	multiplayerLabel->setCharacterSize(fontsize);
	multiplayerLabel->setString("multiplayer");
    multiplayerLabel->normalColor = sf::Color::White;
    multiplayerLabel->focusedColor = sf::Color::Yellow;
    multiplayerLabel->paddingTop = -2;
    multiplayerLabel->paddingLeft = 5;
    multiplayerLabel->paddingRight = 5;
    multiplayerLabel->paddingBottom = 6;
    multiplayerLabel->opacity = 0;
    mainMenu->addChild(multiplayerLabel);


    // Options
    optionsLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::Center);
    optionsLabel->canHaveFocus = true;
    optionsLabel->setFont(font);
	optionsLabel->setCharacterSize(fontsize);
	optionsLabel->setString("settings");
	optionsLabel->normalColor = sf::Color::White;
    optionsLabel->focusedColor = sf::Color::Yellow;
    optionsLabel->paddingTop = -2;
    optionsLabel->paddingLeft = 5;
    optionsLabel->paddingRight = 5;
    optionsLabel->paddingBottom = 6;
    optionsLabel->opacity = 0;
    mainMenu->addChild(optionsLabel);

    // Exit
    exitLabel = new bit::Label(0, -600, 0, 0, bit::Element::AnchorType::Center);
    exitLabel->canHaveFocus = true;
    exitLabel->setFont(font);
	exitLabel->setCharacterSize(fontsize);
	exitLabel->setString("exit");
	exitLabel->normalColor = sf::Color::White;
    exitLabel->focusedColor = sf::Color::Yellow;
    exitLabel->paddingTop = -2;
    exitLabel->paddingLeft = 5;
    exitLabel->paddingRight = 5;
    exitLabel->paddingBottom = 6;
    exitLabel->opacity = 0;
    mainMenu->addChild(exitLabel);

    int delay = 2000;
    inflowLabel(singleplayerLabel, 0, -600, delay + 1000, 0, 520);
    inflowLabel(multiplayerLabel, 0, -600,  delay + 1200, 0, 580);
    inflowLabel(optionsLabel, 0, -600,  delay + 1400, 0, 640);
    inflowLabel(exitLabel, 0, -600,  delay + 1600, 0, 700);
}

bool StateGameStart::update(sf::Time &gameTime)
{
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        requestStateClear();
    }
    else if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Return))
    {
        requestStateClear();
        requestStackPush(RogueZombieGame::stateGamePlayHost);
    }
    else if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Delete))
    {
        requestStateClear();
        requestStackPush(RogueZombieGame::stateGamePlayClient);
    }

    mainMenu->update(*rogueZombieGame->renderWindow, gameTime);

    return true;
}

void StateGameStart::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::State::draw(window, gameTime);

    mainMenu->draw(window, gameTime);
}

void StateGameStart::drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera)
{
}


void StateGameStart::inflowLabel(bit::Element* element, float startingX, float startingY, float delay, float endX, float endY)
{
    element->relativePosition.x = startingX;
    element->relativePosition.y = startingY;
    element->opacity = 0;
    element->queueEffect(new bit::MoveEffect(delay, endX, endY))->queueEffect(new bit::FadeEffect(500, 1));
}