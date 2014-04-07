#pragma once
#ifndef RZ_STARTMAINMENU_H
#define RZ_STARTMAINMENU_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Gui.hpp"

class RogueZombieGame;
class StateGameStart;

class StartMainMenu : public bit::Container
{
public:
    StartMainMenu(RogueZombieGame* rogueZombieGame, StateGameStart* state);

    RogueZombieGame* rzGame;
    StateGameStart* state;
    
    bit::Picture* titlePicture;
    bit::Label* singleplayerLabel;
    bit::Label* multiplayerLabel;
    bit::Label* settingsLabel;
    bit::Label* exitLabel;

    sf::Color rzRed;
    

    void load();

    void showMainMenu(int additionalDelay = 0);

    void hideMainMenu(int additionalDelay = 0);


    void configureLabel(bit::Label* label, std::string text);

    void inflowLabel(bit::Element* element, float startingX, float startingY, float delay, float endX, float endY);

    void outflowLabel(bit::Element* element, float delay);


    bool onActivate_Settings(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime);


};

#endif