#pragma once
#ifndef RZ_HUD_H
#define RZ_HUD_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "Minimap.hpp"

class StateGamePlay;
class OptionsBar;
class Journal;

class Hud : public bit::Container
{
public:

    Hud(StateGamePlay* state);

    StateGamePlay* state;
    OptionsBar* optionsBar;
    Journal* journal;
    Minimap minimap;
    sf::Texture interfaceTexture;
    bit::VertexMap interfaceVertexMap;
    sf::Font journalFont;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

	void draw(sf::RenderWindow &window, sf::Time &gameTime);

    bool typicalContainerControl(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime);

    bool typicalElementControl(Element* element, sf::RenderWindow* window, sf::Time* gameTime);
};

#endif