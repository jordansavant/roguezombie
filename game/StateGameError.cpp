#include "StateGameError.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Input.hpp"
#include "../ResourcePath.h"
#include "RogueZombieGame.hpp"

StateGameError::StateGameError(bit::StateStack &stack, RogueZombieGame* _game)
    : bit::State(stack, _game), rogueZombieGame(_game)
{
    createCamera(rogueZombieGame, 0, 0, 1, 1);

    std::string titleImagePath(resourcePath() + "minititle.png");
    titleTexture.loadFromFile(titleImagePath);
    titleTexture.setSmooth(true);

    std::string fontPath(resourcePath() + "homespun.ttf");
    menuFont.loadFromFile(fontPath);
    menuFontSize = 48;

    errorGui = new bit::Container(0, 0, rogueZombieGame->targetResolution.x, rogueZombieGame->targetResolution.y, bit::Element::AnchorType::Center,
        [] (bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
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
        }
    );
    errorGui->canHaveFocus = true;
    errorGui->isInfocus = true;

    errorMessageLabel = new bit::Label(0, -25, 0, 0, bit::Element::AnchorType::Center);
    errorMessageLabel->canHaveFocus = false;
    errorMessageLabel->setFont(menuFont);
    errorMessageLabel->setCharacterSize(menuFontSize);
    errorMessageLabel->setString(rogueZombieGame->errorMessage);
	errorMessageLabel->normalColor = sf::Color::White;
    errorMessageLabel->focusedColor = sf::Color::White;
    errorMessageLabel->paddingTop = 0;
    errorMessageLabel->paddingLeft = 5;
    errorMessageLabel->paddingRight = 5;
    errorMessageLabel->paddingBottom = 25;
    errorMessageLabel->opacity = 1;
    errorGui->addChild(errorMessageLabel);

    State* state = this;
    RogueZombieGame* rzGame = rogueZombieGame;
    okLabel = new bit::Label(0, 25, 0, 0, bit::Element::AnchorType::Center,
        [rzGame, state] (bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
        {
            if(rzGame->inputManager->isButtonReleased(sf::Mouse::Left))
            {
                state->requestStateClearTo(RogueZombieGame::stateGameStart);
                return true;
            }

            return false;
        }
    );
    okLabel->canHaveFocus = true;
    okLabel->setFont(menuFont);
    okLabel->setCharacterSize(menuFontSize);
	okLabel->setString("ok");
	okLabel->normalColor = sf::Color::Red;
    okLabel->focusedColor = sf::Color::Red;
    okLabel->paddingTop = 0;
    okLabel->paddingLeft = 5;
    okLabel->paddingRight = 5;
    okLabel->paddingBottom = 25;
    okLabel->opacity = 1;
    errorGui->addChild(okLabel);
}

StateGameError::~StateGameError()
{
    delete errorGui;
}

bool StateGameError::update(sf::Time &gameTime)
{
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape) || rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Return))
    {
        requestStateClearTo(RogueZombieGame::stateGameStart);
    }

    errorGui->update(*rogueZombieGame->renderWindow, gameTime);

    return true;
}

void StateGameError::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::State::draw(window, gameTime);

    errorGui->draw(window, gameTime);
}
