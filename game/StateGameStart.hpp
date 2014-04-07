#pragma once
#ifndef RZ_STARTMENUSTATE_H
#define RZ_STARTMENUSTATE_H

#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Gui.hpp"

class RogueZombieGame;

class StateGameStart : public bit::State
{
public:

    StateGameStart(bit::StateStack &stack, RogueZombieGame* game);

    virtual ~StateGameStart();

    RogueZombieGame* rogueZombieGame;

	sf::Font font;
    sf::Texture titleTexture;

    // Main Menu
    bit::Container* mainMenu;
    bit::Picture* titlePicture;
    bit::Label* singleplayerLabel;
    bit::Label* multiplayerLabel;
    bit::Label* optionsLabel;
    bit::Label* exitLabel;

    virtual bool update(sf::Time &gameTime);

    virtual void draw(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera);

    void inflowLabel(bit::Element* element, float startingX, float startingY, float delay, float endX, float endY);

};

#endif
