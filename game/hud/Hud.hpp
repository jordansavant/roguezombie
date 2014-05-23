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
class DialogMenu;
class LootMenu;
class Console;
class TurnQueue;

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
    DialogMenu* dialogMenu;
    LootMenu* lootMenu;
    Console* console;
    TurnQueue* turnQueue;
    Minimap minimap;
    sf::Texture interfaceTexture;
    bit::VertexMap interfaceVertexMap;
    sf::Font journalFont;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

	void draw(sf::RenderWindow &window, sf::Time &gameTime);

    void hideAllMenus(Element* except = NULL);

    void activateJournal(bool hideIfShowing = true);

    void deactivateJournal();

    void activateInventory(bool hideIfShowing = true);

    void deactivateInventory();

    void displayMessage(std::string &message);

    bool typicalContainerControl(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime);

    bool typicalElementControl(Element* element, sf::RenderWindow* window, sf::Time* gameTime);
};

#endif