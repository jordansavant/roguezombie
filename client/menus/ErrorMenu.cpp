#include "ErrorMenu.hpp"
#include <functional>
#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Input.hpp"
#include "../RogueZombieGame.hpp"
#include "../StateGameError.hpp"
#include "StateMenu.hpp"

ErrorMenu::ErrorMenu(RogueZombieGame* rogueZombieGame, StateGameError* state)
    : StateMenu(rogueZombieGame), state(state)
{
    errorMessageLabel = new bit::Label(0, -25, 0, 0, bit::Element::AnchorType::Center);
    configureLabel(errorMessageLabel, rogueZombieGame->errorMessage);
    errorMessageLabel->canHaveFocus = false;
    errorMessageLabel->opacity = 1;
    addChild(errorMessageLabel);

    okLabel = new bit::Label(0, 25, 0, 0, bit::Element::AnchorType::Center,
        [rogueZombieGame, state] (bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
        {
            if(rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left))
            {
                state->requestStateClearTo(RogueZombieGame::stateGameStart);
                return true;
            }

            return false;
        }
    );
    configureLabel(okLabel, "ok");
    okLabel->opacity = 1;
    okLabel->normalColor = rzRed;
}