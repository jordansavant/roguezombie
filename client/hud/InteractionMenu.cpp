#include "InteractionMenu.hpp"
#include "Hud.hpp"
#include "Frame.hpp"
#include "../../server/ClientRequest.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Network.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"
#include "../../server/Interaction.hpp"
#include "../TileClient.hpp"
#include <sstream>

InteractionMenu::InteractionMenu(Hud* _hud)
    : Frame(_hud, 50, 0, 300, 200, bit::Element::AnchorType::Left, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3), false), isActive(false), tileId(0), tileClient(NULL), hasInteractionUnderway(false)
{
    useBottomPointer = true;
    managesOpacity = true;
    opacity = 0;
    canHaveFocus = false;

}

void InteractionMenu::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::Container::update(window, gameTime);
}

void InteractionMenu::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    Frame::updateTargets(window, gameTime);
}

void InteractionMenu::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(tileClient)
    {
        sf::Vector2i mapping = window.mapCoordsToPixel(sf::Vector2f(tileClient->centerRenderX, tileClient->centerRenderY));
        left = (int)(mapping.x - width / 2);
        top = (int)(mapping.y - height - tileClient->height * hud->state->getZoomRatio());
    }
    else
    {
        left = 100000;
    }
    Frame::updateReals(window, gameTime);
}

void InteractionMenu::activate()
{
    isActive = true;
    show();
}

void InteractionMenu::deactivate()
{
    tileId = 0;
    tileClient = NULL;
    isActive = false;
    clearChildren();

    hide();
}

void InteractionMenu::hide()
{
    canHaveFocus = false;
    clearEffects();
    immediateEffect(new bit::FadeEffect(150, .5, bit::Easing::Linear));
}

void InteractionMenu::show()
{
    canHaveFocus = true;
    clearEffects();
    immediateEffect(new bit::FadeEffect(150, Hud::popupOpacity));
}

void InteractionMenu::handleNumberInput(unsigned int number)
{
    unsigned int index = number - 1;
    if (index < childElements.size())
    {
        bit::Element* e = childElements[index];
        e->onActivate(e);
    }
}

void InteractionMenu::handleInteractionTree(bit::ServerPacket &packet, unsigned int tileId)
{
    unsigned int optionSize;
    packet >> optionSize;

    clearChildren();

    this->tileId = tileId;
    this->tileClient = hud->state->levelClient->tiles[tileId];
    
    int y = 10;
    int w = 1;
    int fontSize = 24;
    for(unsigned int i=0; i < optionSize; i++)
    {
        InteractionMenu* m = this;
        Interaction::Type it;
        bit::NetworkHelper::unpackEnum<sf::Uint32, Interaction::Type>(packet, it);
        std::stringstream ss;
        std::string title;
        ss << i + 1 << ". " << Interaction::getTitle(it);
        title = ss.str();
        
        bit::Label* option = new bit::Label(15, y, 0, 0, bit::Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        option->setSfFontSize(fontSize);
        option->setSfFont(hud->journalFont);
        option->normalColor = sf::Color(0, 255, 0);
        option->focusedColor = sf::Color(255, 255, 255);
        option->setSfFontString(title);
        option->canHaveFocus = true;
        option->paddingRight = 10;
        option->paddingBottom = 10;
        option->opacity = 0;
        option->onActivate = [this, it, m, tileId] (Element* e){

            // Do not listen to interaction events if server is still processing a previous request
            if (this->hasInteractionUnderway)
            {
                return;
            }

            this->hasInteractionUnderway = true;
            InteractionMenu* mx = m;
            Interaction::Type itx(it);
            unsigned int tileIdx = tileId;
            m->hud->state->serverRequest(
                [this, itx, tileIdx, mx] (bit::ClientPacket &packet) {
                    packet << sf::Uint32(ClientRequest::ProcessInteractionForBodyOnTile);
                    packet << sf::Uint32(itx);
                    packet << sf::Uint32(tileIdx);
                },
                [this, itx, tileIdx, mx] (bit::ServerPacket &packet) {
                    mx->hud->state->handleInteractionResponse(tileIdx, itx, packet);
                    this->hasInteractionUnderway = false;
                }
            );
        };
        addChild(option);

        int wc = title.size();
        w = std::max(w, wc);

        y += 30;
    }


    // change size based on occurences
    float targetCharWidth = .55f;
    targetHeight = 40 + fontSize * optionSize;
    targetWidth = 60 + (int)((float)(fontSize * w * targetCharWidth));
}