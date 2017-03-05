#include "JoiningMenu.hpp"
#include "../../bitengine/Gui.hpp"
#include "../RogueZombieGame.hpp"
#include "../StateGamePlay.hpp"
#include "StateMenu.hpp"

JoiningMenu::JoiningMenu(RogueZombieGame* rogueZombieGame, StateGamePlay* state)
    : StateMenu(rogueZombieGame), state(state)
{
    joiningLabel = new bit::Label(0, -25, 0, 0, bit::Element::AnchorType::Center);
    configureLabel(joiningLabel, std::string("Server is building world..."));
    joiningLabel->canHaveFocus = false;
    joiningLabel->opacity = 1;
}

void JoiningMenu::hide()
{
    relativePosition.x = 10000;
}