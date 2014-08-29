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
class StatBubble;
class DialogMenu;
class LootMenu;
class Console;
class TurnQueue;
class ActionBar;
class Tooltip;
class InventoryIcon;

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
    StatBubble* statBubble;
    DialogMenu* dialogMenu;
    LootMenu* lootMenu;
    Console* console;
    TurnQueue* turnQueue;
    ActionBar* actionBar;
    Tooltip* tooltip;
    Minimap minimap;
    sf::Texture interfaceTexture;
    bit::VertexMap interfaceVertexMap;
    sf::Font journalFont;
    bit::Pool<InventoryIcon> inventoryIconPool;
    static float hoverlessOpacity;
    static float popupOpacity;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void draw(sf::RenderWindow &window, sf::Time &gameTime);

    void hide();

    void show();

    void hideAllMenus(Element* except = NULL);

    void activateJournal(bool hideIfShowing = true);

    void deactivateJournal();

    void activateInventory(bool hideIfShowing = true);

    void deactivateInventory();

    void onEnterCombat();

    void onLeaveCombat();

    void displayMessage(std::string &message);

    void displayTooltipAt(std::string &info, int screenX, int screenY);

    void hideTooltip();

    bool typicalContainerControl(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime);

    bool typicalElementControl(Element* element, sf::RenderWindow* window, sf::Time* gameTime);

    InventoryIcon* createInventoryIcon();
};

#endif