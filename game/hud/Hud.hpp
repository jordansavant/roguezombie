#pragma once
#ifndef RZ_HUD_H
#define RZ_HUD_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "Minimap.hpp"

class StateGamePlay;
class HudMenu;
class OptionsBar;
class Journal;
class Inventory;
class InteractionMenu;
class LootMenu;
class Console;
class Frame;

class Hud : public bit::Container
{
public:

    Hud(StateGamePlay* state);

    StateGamePlay* state;
    std::vector<HudMenu*> submenus;
    OptionsBar* optionsBar;
    Journal* journal;
    Inventory* inventory;
    InteractionMenu* interactionMenu;
    LootMenu* lootMenu;
    Console* console;
    Minimap minimap;
    sf::Texture interfaceTexture;
    bit::VertexMap interfaceVertexMap;
    sf::Font journalFont;
    Frame* frame;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

	void draw(sf::RenderWindow &window, sf::Time &gameTime);

    void hideAllMenus(Element* except = NULL);

    void activateJournal(bool hideIfShowing = true);

    void activateInventory(bool hideIfShowing = true);

    void displayMessage(std::string &message);

    bool typicalContainerControl(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime);

    bool typicalElementControl(Element* element, sf::RenderWindow* window, sf::Time* gameTime);
};

#endif