#include "JoiningMenu.hpp"
#include "../../bitengine/Gui.hpp"
#include "../RogueZombieGame.hpp"
#include "../StateGamePlay.hpp"
#include "StateMenu.hpp"

JoiningMenu::JoiningMenu(RogueZombieGame* rogueZombieGame, StateGamePlay* state)
    : StateMenu(rogueZombieGame), state(state), messageTimer(.5f), currentMessage(0)
{
    messages.push_back("Server is consulting XoGeni...");
    messages.push_back("Server is seeding life...");
    messages.push_back("Server is building rooms...");
    messages.push_back("Server is building doors...");
    messages.push_back("Server is building tunneling...");
    messages.push_back("Server is building structures...");
    messages.push_back("Server is building characters...");

    joiningLabel = new bit::Label(0, -25, 0, 0, bit::Element::AnchorType::Center);
    configureLabel(joiningLabel, messages[currentMessage]);
    joiningLabel->canHaveFocus = false;
    joiningLabel->opacity = 0;
}

void JoiningMenu::show()
{
    joiningLabel->immediateEffect(new bit::FadeEffect(1000, 1));
}

void JoiningMenu::hide()
{
    bit::FadeEffect* fadeOut = new bit::FadeEffect(500, 0);
    fadeOut->onComplete = [this] (bit::Element* el, bit::Effect* ef) {
        this->relativePosition.x = 10000;
    };
    joiningLabel->immediateEffect(fadeOut);
}

void JoiningMenu::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    StateMenu::update(window, gameTime);

    if(messageTimer.update(gameTime))
    {
        currentMessage = (currentMessage + 1) % messages.size();
        joiningLabel->setSfFontString(messages[currentMessage]);
    }
}