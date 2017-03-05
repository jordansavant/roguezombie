#pragma once
#ifndef RZ_JOININGMENU_H
#define RZ_JOININGMENU_H

#include "../../bitengine/Gui.hpp"
#include "StateMenu.hpp"

class RogueZombieGame;
class StateGamePlay;

class JoiningMenu : public StateMenu
{
public:

    JoiningMenu(RogueZombieGame* rogueZombieGame, StateGamePlay* state);

    StateGamePlay* state;
    bit::Label* joiningLabel;

    void hide();
};

#endif