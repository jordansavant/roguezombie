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
    std::vector<std::string> messages;
    bit::GameTimer messageTimer;
    unsigned int currentMessage;

    void show();

    void hide();

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

};

#endif