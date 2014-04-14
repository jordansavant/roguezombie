#include "OptionsBar.hpp"
#include "HudElement.hpp"
#include "../../bitengine/Input.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../RogueZombieGame.hpp"

OptionsBar::OptionsBar(StateGamePlay* _state)
    : bit::Container(50, 50, _state->rogueZombieGame->targetResolution.x, _state->rogueZombieGame->targetResolution.y, bit::Element::AnchorType::TopLeft), state(_state)
{
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

    options = new HudElement(0, 0, 0, 0, Element::AnchorType::TopLeft,
        [_state] (Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
        {
            int centerX = element->left + element->width / 2;
            int centerY = element->top + element->height / 2;
            if(bit::VectorMath::inRadius(_state->levelClient->mousePositionInScreen.x, _state->levelClient->mousePositionInScreen.y, centerX, centerY, element->width / 2))
            {
                if(_state->rogueZombieGame->inputManager->isButtonPressed(sf::Mouse::Left))
                {
                    return true;
                }
            }

            return false;
        }
    );
    options->load(state, std::string("optionbar_options"));
    addChild(options);

    
    journal = new HudElement(64, 0, 0, 0, Element::AnchorType::TopLeft,
        [_state] (Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
        {
            int centerX = element->left + element->width / 2;
            int centerY = element->top + element->height / 2;
            if(bit::VectorMath::inRadius(_state->levelClient->mousePositionInScreen.x, _state->levelClient->mousePositionInScreen.y, centerX, centerY, element->width / 2))
            {
                if(_state->rogueZombieGame->inputManager->isButtonPressed(sf::Mouse::Left))
                {
                    return true;
                }
            }

            return false;
        }
    );
    journal->load(state, std::string("optionbar_journal"));
    addChild(journal);

    
    character = new HudElement(128, 0, 0, 0, Element::AnchorType::TopLeft,
        [_state] (Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
        {
            int centerX = element->left + element->width / 2;
            int centerY = element->top + element->height / 2;
            if(bit::VectorMath::inRadius(_state->levelClient->mousePositionInScreen.x, _state->levelClient->mousePositionInScreen.y, centerX, centerY, element->width / 2))
            {
                if(_state->rogueZombieGame->inputManager->isButtonPressed(sf::Mouse::Left))
                {
                    return true;
                }
            }

            return false;
        }
    );
    character->load(state, std::string("optionbar_character"));
    addChild(character);

    
    inventory = new HudElement(192, 0, 0, 0, Element::AnchorType::TopLeft,
        [_state] (Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
        {
            int centerX = element->left + element->width / 2;
            int centerY = element->top + element->height / 2;
            if(bit::VectorMath::inRadius(_state->levelClient->mousePositionInScreen.x, _state->levelClient->mousePositionInScreen.y, centerX, centerY, element->width / 2))
            {
                if(_state->rogueZombieGame->inputManager->isButtonPressed(sf::Mouse::Left))
                {
                    return true;
                }
            }

            return false;
        }
    );
    inventory->load(state, std::string("optionbar_inventory"));
    addChild(inventory);

    
    map = new HudElement(256, 0, 0, 0, Element::AnchorType::TopLeft,
        [_state] (Element* element, sf::RenderWindow* window, sf::Time* gameTime) -> bool
        {
            int centerX = element->left + element->width / 2;
            int centerY = element->top + element->height / 2;
            if(bit::VectorMath::inRadius(_state->levelClient->mousePositionInScreen.x, _state->levelClient->mousePositionInScreen.y, centerX, centerY, element->width / 2))
            {
                if(_state->rogueZombieGame->inputManager->isButtonPressed(sf::Mouse::Left))
                {
                    return true;
                }
            }

            return false;
        }
    );
    map->load(state, std::string("optionbar_map"));
    addChild(map);
}

void OptionsBar::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    state->rogueZombieGame->depthTestBegin();

    // apply the transform
    states.texture = &state->interfaceTexture;
    window.draw(state->interfaceVertexMap.vertexArray, states);

    state->rogueZombieGame->depthTestEnd();
}
