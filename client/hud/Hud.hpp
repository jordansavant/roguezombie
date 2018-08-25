#pragma once
#ifndef RZ_HUD_H
#define RZ_HUD_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Structures.hpp"

class StateGamePlay;
class HudMenu;
class HudElement;
class OptionsBar;
class Journal;
class Options;
class Inventory;
class InteractionMenu;
class StatBubble;
class DialogMenu;
class LootMenu;
class Console;
class DevTerminal;
class TurnQueue;
class ActionBar;
class Tooltip;
class InventoryIcon;
class Minimap;

class Hud : public bit::Container
{
public:

    Hud(StateGamePlay* state);

    virtual ~Hud();

    StateGamePlay* state;
    std::vector<HudMenu*> submenus;
    OptionsBar* optionsBar;
    Journal* journal;
    Options* options;
    Inventory* inventory;
    InteractionMenu* interactionMenu;
    StatBubble* statBubble;
    DialogMenu* dialogMenu;
    LootMenu* lootMenu;
    Console* console;
	DevTerminal* devTerminal;
    TurnQueue* turnQueue;
    ActionBar* actionBar;
    std::map<unsigned int, Minimap*> minimaps;
    Minimap* liveMinimap;
    bool isMapActive;
    sf::Texture interfaceTexture;
    bit::VertexMap interfaceVertexMap;
    sf::Font journalFont;
    bit::Pool<InventoryIcon> inventoryIconPool;
    unsigned int slotAcceptSoundId;
    HudElement* closeIconElement;
    std::vector<Tooltip*> toolTips;

    static bool destroying;
    static float hoverlessOpacity;
    static float popupOpacity;
    static float zindex_minimap;
    static float zindex_icons;
    static float zindex_iconsDragged;
    static float zindex_cells;
    static float zindex_frames;
    static int font_largeSize;
    static int font_primarySize;
    static int font_noteSize;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void draw(sf::RenderWindow &window, sf::Time &gameTime);

    void hide();

    void show();

    void hideAllMenus(Element* except = NULL);

    void activateJournal(bool hideIfShowing = true);

    void deactivateJournal();

    void activateOptions(bool hideIfShowing = true);

    void deactivateOptions();

    void activateInventory(bool hideIfShowing = true);

    void deactivateInventory();

    void activateActionBar();

    void deactivateActionBar();

	void activateDevTerminal();

	void deactivateDevTerminal();

    void showCloseButton();

    void hideCloseButton();

    void toggleMap();

    void onEnterCombat();

    void onLeaveCombat();

    void displayMessage(std::string const&message);

    void displayTooltipAt(std::string const&info, std::string const&tag, int screenX, int screenY, float opacity = 1, int delay = 750, int duration = 3000, int width = -1, int height = -1);

    void hideTooltip(std::string const&tag);

    bool canMoveInventory();

    bool typicalContainerControl(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime);

    bool typicalElementControl(Element* element, sf::RenderWindow* window, sf::Time* gameTime);

    InventoryIcon* createInventoryIcon();

    void playSlotAcceptSound();

    void onLevelChange(unsigned int oldLevelId, unsigned int newLevelId);

    static float getDrawDepth(float targetDepth);
};

#endif