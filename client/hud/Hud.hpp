#pragma once
#ifndef RZ_HUD_H
#define RZ_HUD_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "Minimap.hpp"

class StateGamePlay;
class HudMenu;
class HudElement;
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

    virtual ~Hud();

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
    unsigned int slotDenialSoundId;
    HudElement* closeIconElement;

    static bool destroying;
    static float hoverlessOpacity;
    static float popupOpacity;
    static float zindex_icons;
    static float zindex_iconsDragged;
    static float zindex_cells;
    static float zindex_frames;
    static int font_primarySize;
    static int font_noteSize;
    static sf::Color font_primaryColor;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void draw(sf::RenderWindow &window, sf::Time &gameTime);

    void hide();

    void show();

    void hideAllMenus(Element* except = NULL);

    void activateJournal(bool hideIfShowing = true);

    void deactivateJournal();

    void activateInventory(bool hideIfShowing = true);

    void deactivateInventory();

    void activateActionBar();

    void deactivateActionBar();

    void showCloseButton();

    void hideCloseButton();

    void onEnterCombat();

    void onLeaveCombat();

    void displayMessage(std::string &message);

    void displayTooltipAt(std::string &info, int screenX, int screenY, float opacity = 1, int delay = 750, int duration = 3000, int width = -1, int height = -1);

    void hideTooltip();

    bool canMoveInventory();

    bool typicalContainerControl(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime);

    bool typicalElementControl(Element* element, sf::RenderWindow* window, sf::Time* gameTime);

    InventoryIcon* createInventoryIcon();

    void playSlotDenialSound();

    static float getDrawDepth(float targetDepth);
};

#endif