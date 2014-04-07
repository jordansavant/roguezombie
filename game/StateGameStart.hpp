#pragma once
#ifndef RZ_STARTMENUSTATE_H
#define RZ_STARTMENUSTATE_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Gui.hpp"

class RogueZombieGame;
class StartMainMenu;

class StateGameStart : public bit::State
{
public:

    StateGameStart(bit::StateStack &stack, RogueZombieGame* game);

    virtual ~StateGameStart();

    RogueZombieGame* rogueZombieGame;

	sf::Font menuFont;
    int menuFontSize;
    sf::Texture titleTexture;

    // Main Menu
    StartMainMenu* mainMenu;

    virtual bool update(sf::Time &gameTime);

    virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera);


};

#endif
