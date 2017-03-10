#pragma once
#ifndef RZ_STATEMENU_H
#define RZ_STATEMENU_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Gui.hpp"

class RogueZombieGame;
class StateGameStart;

class StateMenu : public bit::Container
{
public:
    StateMenu(RogueZombieGame* rogueZombieGame);

    RogueZombieGame* rogueZombieGame;
    sf::Color rzRed;
    sf::Color rzGreen;
    sf::Color rzGray;
    sf::Font menuFont;
    int menuFontSize;
    sf::Texture titleTexture;

    void load();

    void configureLabel(bit::Label* label, std::string text);

    void inflowLabel(bit::Element* element, float startingX, float startingY, float delay, float endX, float endY);

    void outflowLabel(bit::Element* element, float delay, float endX, float endY);

    bool onActivate_Settings(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime);
};

#endif