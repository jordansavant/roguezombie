#include "Hud.hpp"
#include "HudElement.hpp"
#include "Minimap.hpp"
#include "OptionsBar.hpp"
#include "Journal.hpp"
#include "Options.hpp"
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
#include "Minimap.hpp"
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

float Hud::zindex_minimap = .08f; // inventory icons
float Hud::zindex_icons = .8f; // inventory icons
float Hud::zindex_iconsDragged = 1.0f; // icons being dragged
float Hud::zindex_cells = .1f; // containers for inventory icons
float Hud::zindex_frames = .1f; // menu frames

int Hud::font_largeSize = 32;
int Hud::font_primarySize = 24;
int Hud::font_noteSize = 20;
sf::Color Hud::font_primaryColor = RZConfig::fontGreen;

Hud::Hud(StateGamePlay* _state)
    : bit::Container(0, 0, _state->rogueZombieGame->targetResolution.x, _state->rogueZombieGame->targetResolution.y, bit::Element::AnchorType::Top, std::bind(&Hud::typicalContainerControl, this, std::placeholders::_1, std::placeholders::_2,  std::placeholders::_3)),
    state(_state), inventoryIconPool(500, std::bind(&Hud::createInventoryIcon, this)), liveMinimap(NULL), isMapActive(true)
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

    // Minimaps

    // Hud Menus
    journal = new Journal(this);
    addChild(journal);

    options = new Options(this);
    addChild(options);
    
    inventory = new Inventory(this);
    addChild(inventory);

    // Initialize all menus
    submenus.push_back(inventory);
    submenus.push_back(journal);
    submenus.push_back(options);
    for(unsigned int i=0; i < submenus.size(); i++)
    {
        submenus[i]->hide();
    }

    // Tool tips
    for(unsigned int i = 0; i < 10; i++)
    {
        Tooltip* t = new Tooltip(this);
        addChild(t);
        toolTips.push_back(t);
    }

    // Sounds
    slotAcceptSoundId = state->rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_menu_01.ogg");

    // Close
    closeIconElement = new HudElement(0, -392, 0, 0, bit::Element::AnchorType::Right, std::bind(&Hud::typicalElementControl, this, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    closeIconElement->load(this, std::string("optionbar_x"));
    closeIconElement->onActivate = [this] (Element* e) -> void {
        this->state->exitInventoryModes();
    };
    closeIconElement->opacity = 0;
    addChild(closeIconElement);
}

Hud::~Hud()
{
    destroying = true;

    for(auto iterator = minimaps.begin(); iterator != minimaps.end(); iterator++)
    {
        delete iterator->second;
    }
}

void Hud::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::Container::update(window, gameTime);
    this->actionBar;

    // Minimap
    if(liveMinimap)
    {
        liveMinimap->update(gameTime);
    }
}

void Hud::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    sf::RenderStates states;
    state->rogueZombieGame->depthTestBegin();

    // Hud Icons
    states.texture = &interfaceTexture;
    window.draw(interfaceVertexMap.vertexArray, states);

    // Minimap
    if(liveMinimap && isMapActive)
    {
        states.transform *= liveMinimap->getTransform();
        window.draw(liveMinimap->vertexMap.vertexArray, states);
    }

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

    hideCloseButton();
}

void Hud::activateJournal(bool hideIfShowing)
{
    if(!journal->isShown)
    {
        hideAllMenus(journal);
        journal->show();
        showCloseButton();
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
        hideCloseButton();
    }
}

void Hud::activateOptions(bool hideIfShowing)
{
    if(!options->isShown)
    {
        hideAllMenus(options);
        options->show();
        showCloseButton();
    }
    else if(hideIfShowing)
    {
        hideAllMenus();
    }
}

void Hud::deactivateOptions()
{
    if(options->isShown)
    {
        options->hide();
        hideCloseButton();
    }
}

void Hud::activateInventory(bool hideIfShowing)
{
    if(!inventory->isShown)
    {
        hideAllMenus(inventory);
        inventory->show();
        showCloseButton();
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
        hideCloseButton();
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

void Hud::showCloseButton()
{
    closeIconElement->relativePosition.x = 100;
    closeIconElement->show(new bit::FadeEffect(150, 1));
    closeIconElement->immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, -100, 0));
}

void Hud::hideCloseButton()
{
    closeIconElement->relativePosition.x = 0;
    closeIconElement->hide(new bit::FadeEffect(150, 0));
    closeIconElement->immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, 100, 0));
}

void Hud::displayMessage(std::string &message)
{
    console->print(message);
}

void Hud::displayTooltipAt(std::string &info, std::string &tag, int screenX, int screenY, float opacity, int delay, int duration, int width, int height)
{
    // Find an inactive tooltip
    for(unsigned int i = 0; i < toolTips.size(); i++)
    {
        if(!toolTips[i]->isActive)
        {
            toolTips[i]->displayAt(info, tag, screenX, screenY, opacity, delay, duration, width, height);

            break;
        }
    }
}

void Hud::hideTooltip(std::string &tag)
{
    // Find tooltip and deactivate
    for(unsigned int i = 0; i < toolTips.size(); i++)
    {
        if(toolTips[i]->tag == tag)
        {
            toolTips[i]->deactivate();

            break;
        }
    }
}

void Hud::toggleMap()
{
    isMapActive = !isMapActive;
}

bool Hud::canMoveInventory()
{
    return state->mode == StateGamePlay::Mode::Inventory || state->mode == StateGamePlay::Mode::Loot;
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

    // Dont activate if we are a relatively new kid on the block
    if(element->timeInParent <= sf::seconds(.5))
    {
        return false;
    }

    // Do not consider activation attempts if we are dragging considerably
    if(element->draggable)
    {
        if(element->draggable->isSignificantDrag())
        {
            return false;
        }
    }

    if(bit::VectorMath::inRadius(state->levelClient->mousePositionInScreen.x, state->levelClient->mousePositionInScreen.y, centerX, centerY, element->width / 2))
    {
        if(state->rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left))
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

void Hud::playSlotAcceptSound()
{
    state->rogueZombieGame->soundManager->play(slotAcceptSoundId);
}

void Hud::onLevelChange(unsigned int oldLevelId, unsigned int newLevelId)
{
    // Find the old map and hide it
    auto curIter = minimaps.find(newLevelId);
    if(curIter != minimaps.end())
    {
        Minimap* currentMinimap = curIter->second;
        currentMinimap->move(10000,0);
    }

    // Find the new map and show it or create it
    auto newIter = minimaps.find(newLevelId);
    if(newIter != minimaps.end())
    {
        Minimap* newMinimap = newIter->second;
        newMinimap->move(-10000,0);
        liveMinimap = newMinimap;
    }
    else
    {
        Minimap* minimap = new Minimap();
        minimap->load(this);
        minimaps.insert(std::pair<unsigned int, Minimap*>(newLevelId, minimap));
        liveMinimap = minimap;
    }
}

float Hud::getDrawDepth(float targetDepth)
{
    // Target depth does not consider game elements
    return 1 - bit::Math::calculateDrawDepthByRange(targetDepth, 1.0f, RZConfig::zrenderInterfaceLowerbound, RZConfig::zrenderInterfaceUpperbound);
}