#include "Hud.hpp"
#include "Minimap.hpp"
#include "OptionsBar.hpp"
#include "Journal.hpp"
#include "Inventory.hpp"
#include "InteractionMenu.hpp"
#include "StatBubble.hpp"
#include "DialogMenu.hpp"
#include "LootMenu.hpp"
#include "Console.hpp"
#include "TurnQueue.hpp"
#include "ActionBar.hpp"
#include "Tooltip.hpp"
#include "InventoryIcon.hpp"
#include "../../ResourcePath.h"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Audio.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../RZConfig.hpp"

bool Hud::destroying = false;
float Hud::hoverlessOpacity = .5f;
float Hud::popupOpacity = .7f;

float Hud::zindex_icons = .8f; // inventory icons
float Hud::zindex_iconsDragged = 1.0f; // icons being dragged
float Hud::zindex_cells = .1f; // containers for inventory icons
float Hud::zindex_frames = .1f; // menu frames

int Hud::font_primarySize = 24;
int Hud::font_noteSize = 20;
sf::Color Hud::font_primaryColor = sf::Color(0, 255, 0);

Hud::Hud(StateGamePlay* _state)
    : bit::Container(0, 0, _state->rogueZombieGame->targetResolution.x, _state->rogueZombieGame->targetResolution.y, bit::Element::AnchorType::Top, std::bind(&Hud::typicalContainerControl, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3)),
      state(_state), inventoryIconPool(500, std::bind(&Hud::createInventoryIcon, this))
{
    destroying = false;
    fullscreen = true;

    // Assets
    state->rogueZombieGame->spriteLoader->loadSprites(resourcePath() + "interface_01.csv");
    interfaceTexture.loadFromFile(resourcePath() + "interface_01.png");
    interfaceTexture.setSmooth(false);
    interfaceVertexMap.load(&interfaceTexture, sf::PrimitiveType::Quads);
    journalFont.loadFromFile(resourcePath() + "OCRAEXT.TTF");
    
    console = new Console(this);
    addChild(console);

    turnQueue = new TurnQueue(this);
    addChild(turnQueue);
    
    actionBar = new ActionBar(this);
    addChild(actionBar);

    // Special Menus
    optionsBar = new OptionsBar(this);
    addChild(optionsBar);

    interactionMenu = new InteractionMenu(this);
    addChild(interactionMenu);

    statBubble = new StatBubble(this);
    addChild(statBubble);
    
    dialogMenu = new DialogMenu(this);
    addChild(dialogMenu);

    lootMenu = new LootMenu(this);
    addChild(lootMenu);

    minimap.load(this);

    // Hud Menus
    journal = new Journal(this);
    addChild(journal);
    
    inventory = new Inventory(this);
    addChild(inventory);

    // Initialize all menus
    submenus.push_back(inventory);
    submenus.push_back(journal);
    for(unsigned int i=0; i < submenus.size(); i++)
    {
        submenus[i]->canHaveFocus = false;
        submenus[i]->opacity = 0;
    }

    tooltip = new Tooltip(this);
    addChild(tooltip);

    // Sounds
    slotDenialSoundId = state->rogueZombieGame->soundManager->loadSound(resourcePath() + "slotbeep_688997_SOUNDDOGS__of.ogg");
}

Hud::~Hud()
{
    destroying = true;
}

void Hud::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::Container::update(window, gameTime);

    // Minimap
    float scale = bit::Math::roundPowerOf2(state->rogueZombieGame->currentResolutionRatio);
    //minimap.setPosition(175 * state->rogueZombieGame->currentResolutionRatioX, 125 * state->rogueZombieGame->currentResolutionRatioY);
    //minimap.setScale(scale, scale);
}

void Hud::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    sf::RenderStates states;
    state->rogueZombieGame->depthTestBegin();

    // Hud Icons
    states.texture = &interfaceTexture;
    window.draw(interfaceVertexMap.vertexArray, states);

    // Minimap
    //states.transform *= minimap.getTransform();
    //window.draw(minimap.vertexMap.vertexArray, states);

    state->rogueZombieGame->depthTestEnd();

    bit::Container::draw(window, gameTime);
}


void Hud::hide()
{
    // TODO, total hack
    relativePosition.x += 10000;
}

void Hud::show()
{
    relativePosition.x -= 10000;
}

void Hud::onEnterCombat()
{
}

void Hud::onLeaveCombat()
{
    turnQueue->hide();
}

void Hud::hideAllMenus(Element* except)
{
    for(unsigned int i=0; i < submenus.size(); i++)
    {
        if(submenus[i] != except && submenus[i]->isShown)
        {
            submenus[i]->hide();
        }
    }
}

void Hud::activateJournal(bool hideIfShowing)
{
    if(!journal->isShown)
    {
        hideAllMenus(journal);
        journal->show();
    }
    else if(hideIfShowing)
    {
        hideAllMenus();
    }
}

void Hud::deactivateJournal()
{
    if(journal->isShown)
    {
        journal->hide();
    }
}

void Hud::activateInventory(bool hideIfShowing)
{
    if(!inventory->isShown)
    {
        hideAllMenus(inventory);
        inventory->show();
    }
    else if(hideIfShowing)
    {
        hideAllMenus();
    }
}

void Hud::deactivateInventory()
{
    if(inventory->isShown)
    {
        inventory->hide();
    }
}

void Hud::activateActionBar()
{
    actionBar->show();
}

void Hud::deactivateActionBar()
{
    actionBar->hide();
}

void Hud::displayMessage(std::string &message)
{
    console->print(message);
}

void Hud::displayTooltipAt(std::string &info, int screenX, int screenY, float opacity, int delay, int duration, int width, int height)
{
    tooltip->displayAt(info, screenX, screenY, opacity, delay, duration, width, height);
}

void Hud::hideTooltip()
{
    tooltip->deactivate();
}

bool Hud::typicalContainerControl(bit::Element* element, sf::RenderWindow* window, sf::Time* gameTime)
{
    bit::Container* container = static_cast<bit::Container*>(element);
    sf::Vector2i pixelPos = sf::Mouse::getPosition(*window);

    if(!container || !this->state->rogueZombieGame->isInFocus)
    {
        return false;
    }

    bool in = false;
    for(int i = 0; i < container->childElements.size(); i++)
    {
        if(container->childElements[i]->canHaveFocus)
        {
            if(container->childElements[i]->contains(pixelPos.x, pixelPos.y))
            {
                in = true;
                container->changeFocusedChild(i);
                break;
            }
        }
    }

    if(!in)
    {
        container->clearFocusedChild();
    }

    // Never block child input
    return false;
}

bool Hud::typicalElementControl(Element* element, sf::RenderWindow* window, sf::Time* gameTime)
{
    int centerX = element->left + element->width / 2;
    int centerY = element->top + element->height / 2;
    if(bit::VectorMath::inRadius(state->levelClient->mousePositionInScreen.x, state->levelClient->mousePositionInScreen.y, centerX, centerY, element->width / 2))
    {
        if(state->rogueZombieGame->inputManager->isButtonPressed(sf::Mouse::Left))
        {
            return true;
        }
    }

    return false;
}

InventoryIcon* Hud::createInventoryIcon()
{
    return new InventoryIcon(this);
}

void Hud::playSlotDenialSound()
{
    state->rogueZombieGame->soundManager->play(slotDenialSoundId);
}

float Hud::getDrawDepth(float targetDepth)
{
    // Target depth does not consider game elements
    return 1 - bit::Math::calculateDrawDepthByRange(targetDepth, 1.0f, RZConfig::zrenderInterfaceLowerbound, RZConfig::zrenderInterfaceUpperbound);
}