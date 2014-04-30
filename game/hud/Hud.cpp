#include "Hud.hpp"
#include "Minimap.hpp"
#include "OptionsBar.hpp"
#include "Journal.hpp"
#include "InteractionMenu.hpp"
#include "../../ResourcePath.h"
#include "../../bitengine/Input.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../RogueZombieGame.hpp"

Hud::Hud(StateGamePlay* _state)
    : bit::Container(0, 0, _state->rogueZombieGame->targetResolution.x, _state->rogueZombieGame->targetResolution.y, bit::Element::AnchorType::Top), state(_state)
{
    // Assets
    state->rogueZombieGame->spriteLoader->loadSprites(resourcePath() + "interface_01.csv");
    interfaceTexture.loadFromFile(resourcePath() + "interface_01.png");
    interfaceVertexMap.load(&interfaceTexture, sf::PrimitiveType::Quads);
    journalFont.loadFromFile(resourcePath() + "homespun.ttf");

    lambdaListenToInput = std::bind(&Hud::typicalContainerControl, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3);
    canHaveFocus = true;
    isInfocus = true;

    optionsBar = new OptionsBar(this);
    addChild(optionsBar);

    journal = new Journal(this);
    addChild(journal);

    interactionMenu = new InteractionMenu(this);
    addChild(interactionMenu);

    minimap.load(this);
}

void Hud::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::Container::update(window, gameTime);

    // Minimap
    float scale = bit::Math::roundPowerOf2(state->rogueZombieGame->currentResolutionRatio);
    minimap.setPosition(175 * state->rogueZombieGame->currentResolutionRatioX, 125 * state->rogueZombieGame->currentResolutionRatioY);
    minimap.setScale(scale, scale);

    journal->update(window, gameTime);
    optionsBar->update(window, gameTime);
    interactionMenu->update(window, gameTime);
}

void Hud::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    sf::RenderStates states;
    state->rogueZombieGame->depthTestBegin();

    // Hud Icons
    states.texture = &interfaceTexture;
    window.draw(interfaceVertexMap.vertexArray, states);

    // Minimap
    states.transform *= minimap.getTransform();
    window.draw(minimap.vertexMap.vertexArray, states);

    state->rogueZombieGame->depthTestEnd();

    bit::Container::draw(window, gameTime);
}

bool Hud::typicalContainerControl(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime)
{
    bit::Container* container = static_cast<bit::Container*>(element);
    sf::Vector2i pixelPos = sf::Mouse::getPosition(*window);

    if(!container || !this->state->rogueZombieGame->isInFocus)
    {
        return false;
    }

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

bool Hud::typicalElementControl(Element* element, sf::RenderWindow* window, sf::Time* gameTime)
{
    int centerX = element->left + element->width / 2;
    int centerY = element->top + element->height / 2;
    if(bit::VectorMath::inRadius(state->levelClient->mousePositionInScreen.x, state->levelClient->mousePositionInScreen.y, centerX, centerY, element->width / 2))
    {
        if(state->rogueZombieGame->inputManager->isButtonPressed(sf::Mouse::Left))
        {
            return true;
        }
    }

    return false;
}