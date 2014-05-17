#include "DialogMenu.hpp"
#include "Hud.hpp"
#include "Frame.hpp"
#include "../ClientRequest.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Network.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../TileClient.hpp"
#include "../dialog/Dialog.hpp"
#include "../dialog/DialogEntry.hpp"

DialogMenu::DialogMenu(Hud* _hud)
    : Frame(_hud, 50, 0, 500, 300, bit::Element::AnchorType::Left, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), isActive(false), tileId(0), tileClient(NULL)
{
    useBottomPointer = true;
    managesOpacity = true;
    opacity = 0;
}

void DialogMenu::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::Container::update(window, gameTime);
}

void DialogMenu::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    Frame::updateTargets(window, gameTime);
}

void DialogMenu::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(tileClient)
    {
        sf::Vector2i mapping = window.mapCoordsToPixel(sf::Vector2f(tileClient->centerRenderX, tileClient->centerRenderY));
        left = mapping.x - width / 2;
        top = mapping.y - height - tileClient->height;
        Frame::updateReals(window, gameTime);
    }
}

void DialogMenu::activate()
{
    isActive = true;
    show();
}

void DialogMenu::deactivate()
{
    tileId = 0;
    tileClient = NULL;
    isActive = false;
    clearChildren();

    hide();
}

void DialogMenu::hide()
{
    canHaveFocus = false;
    clearEffects();
    immediateEffect(new bit::FadeEffect(150, 0, bit::Easing::Linear, [](bit::Element* e, bit::Effect* f){
        bool crap = true;
    }));
}

void DialogMenu::show()
{
    canHaveFocus = true;
    clearEffects();
    immediateEffect(new bit::FadeEffect(150, 1));
}

void DialogMenu::handleDialogPacket(bit::ServerPacket &packet, unsigned int tileId)
{
    clearChildren();

    this->tileId = tileId;
    this->tileClient = hud->state->levelClient->tiles[tileId];

    int y = 10;
    
    // Unpack the prompt
    Dialog prompt;
    packet >> prompt;

    bit::Label* promptLabel = new bit::Label(20, y, 0, 0, bit::Element::AnchorType::TopLeft);
    promptLabel->setSfFontSize(24);
    promptLabel->setSfFont(hud->journalFont);
    promptLabel->normalColor = sf::Color::White;
    promptLabel->setSfFontString(DialogEntry::getString(prompt.entry));
    promptLabel->paddingRight = 10;
    promptLabel->paddingBottom = 10;
    promptLabel->opacity = 0;
    addChild(promptLabel);
    y += 80;

    // Unpack the responses
    unsigned int responseSize;
    packet >> responseSize;
    for(unsigned int i=0; i < responseSize; i++)
    {
        Dialog response;
        packet >> response;

        bit::Label* responseLabel = new bit::Label(20, y, 0, 0, bit::Element::AnchorType::TopLeft);
        responseLabel->setSfFontSize(24);
        responseLabel->setSfFont(hud->journalFont);
        responseLabel->normalColor = sf::Color::White;
        responseLabel->setSfFontString("> " + DialogEntry::getString(response.entry));
        responseLabel->paddingRight = 10;
        responseLabel->paddingBottom = 10;
        responseLabel->opacity = 0;
        addChild(responseLabel);
        y += 30;
    }
}