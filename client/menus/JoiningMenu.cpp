#include "JoiningMenu.hpp"
#include "../../bitengine/Gui.hpp"
#include "../RogueZombieGame.hpp"
#include "../StateGamePlay.hpp"
#include "StateMenu.hpp"

JoiningMenu::JoiningMenu(RogueZombieGame* rogueZombieGame, StateGamePlay* state)
    : StateMenu(rogueZombieGame), state(state), currentMessage(0)
{
    messages.push_back("Server is loading...");
    messages.push_back("Server is loading...");
    messages.push_back("Server is loading...");
    messages.push_back("Server is initializing XoGeni...");
    messages.push_back("Server is seeding life...");
    messages.push_back("Server is planning rooms...");
    messages.push_back("Server is setting doors...");
    messages.push_back("Server is tunneling corridors...");
    messages.push_back("Server is building walls...");
    messages.push_back("Server is placing lights...");
    messages.push_back("Server is spawning characters...");
    messages.push_back("Server is consulting random number god...");
    messages.push_back("Server is hoarding treasure...");
    messages.push_back("Server is balancing enemies...");
    messages.push_back("Server is ensuring connectivity...");
    messages.push_back("Server is hiding loot...");
    messages.push_back("Server is crafting keys...");
    messages.push_back("Server is locking doors...");
    messages.push_back("Server is deftly placing traps...");
    messages.push_back("Server is validating rules...");
    messages.push_back("Server is writing narratives...");
    messages.push_back("Server is assigning objectives...");
    messages.push_back("Server is generating interaction trees...");

    joiningLabel = new bit::Label(0, -25, 0, 0, bit::Element::AnchorType::Center);
    configureLabel(joiningLabel, messages[currentMessage]);
    joiningLabel->canHaveFocus = false;
    joiningLabel->opacity = 0;
}

void JoiningMenu::effectMessage()
{
    bit::FadeEffect* fadeIn = new bit::FadeEffect(300, 1);
    bit::FadeEffect* fadeOut = new bit::FadeEffect(300, 0);
    fadeOut->onComplete = [this] (bit::Element* el, bit::Effect* ef) {
        this->currentMessage = (this->currentMessage + 1) % this->messages.size();
        this->joiningLabel->setSfFontString(this->messages[this->currentMessage]);
        this->effectMessage();
    };
    joiningLabel->queueEffect(fadeIn);
    joiningLabel->queueEffect(new bit::Effect(400)); // pause
    joiningLabel->queueEffect(fadeOut);
}

void JoiningMenu::show()
{
    effectMessage();
}

void JoiningMenu::hide()
{
    joiningLabel->clearEffects();
    bit::FadeEffect* fadeOut = new bit::FadeEffect(500, 0);
    fadeOut->onComplete = [this] (bit::Element* el, bit::Effect* ef) {
        this->relativePosition.x = 10000;
    };
    joiningLabel->immediateEffect(fadeOut);
}
