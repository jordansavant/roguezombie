#pragma once
#ifndef RZ_STARTMAINMENU_H
#define RZ_STARTMAINMENU_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "StateMenu.hpp"

class RogueZombieGame;
class StateGameStart;

class StartMainMenu : public StateMenu
{
public:

    StartMainMenu(RogueZombieGame* rogueZombieGame, StateGameStart* state);

    StateGameStart* state;
    bit::Container* centroid;
    bit::Picture* titlePicture;
    bit::Label* singleplayerLabel;
    bit::Label* multiplayerLabel;
    bit::Label* settingsLabel;
    bit::Label* exitLabel;

    void load();

    void showMainMenu(int additionalDelay = 0);

    void hideMainMenu(int additionalDelay = 0);

    bool onActivate_Settings(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime);
};

#endif