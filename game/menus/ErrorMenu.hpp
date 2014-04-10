#pragma once
#ifndef RZ_ERRORMENU_H
#define RZ_ERRORMENU_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "StateMenu.hpp"

class RogueZombieGame;
class StateGameError;

class ErrorMenu : public StateMenu
{
public:

    ErrorMenu(RogueZombieGame* rogueZombieGame, StateGameError* state);

    StateGameError* state;
    bit::Label* errorMessageLabel;
    bit::Label* okLabel;

};

#endif