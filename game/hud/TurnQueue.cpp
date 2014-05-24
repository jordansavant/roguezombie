#include "TurnQueue.hpp"
#include "Hud.hpp"
#include "HudElement.hpp"
#include "../Character.hpp"
#include "../CharacterSprite.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"

TurnQueue::TurnQueue(Hud* _hud)
    : bit::Container(-50, -50, 20, 128, bit::Element::AnchorType::BottomRight, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), hud(_hud)
{
    scaleStyle = ScaleStyle::None;
    
    // Fill elements with turn queue items
    for(unsigned int i=0; i < 20; i++)
    {
        HudElement* he = new HudElement(0, 10000, 0, 0, Element::AnchorType::Bottom);
        he->load(hud, std::string("optionbar_character"));
        he->scaleStyle = Element::ScaleStyle::None;
        addChild(he);
    }

    //targetWidth = originX;
    //targetHeight = primary->sprite->height;
}

void TurnQueue::handleTurnQueue(bit::ServerPacket &packet)
{
    // Get the count
    unsigned int size;
    packet >> size;

    int y=0;
    hide();
    for(unsigned int i=0; i < size; i++)
    {
        bool alive;
        packet >> alive;

        if(alive)
        {
            Character::Type type;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Character::Type>(packet, type);

            HudElement* he = static_cast<HudElement*>(childElements[i]);
            he->sprite = hud->state->rogueZombieGame->spriteLoader->getSprite("tq_" + CharacterSprite::getSpriteMoniker(type) + "_active");
            he->sprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[he->quadIndex]);
            he->scaleStyle = Element::ScaleStyle::None;
            he->relativePosition.y = y;

            y -= 20;
        }
    }
}

void TurnQueue::hide()
{
    for(unsigned int i=0; i < childElements.size(); i++)
    {
        childElements[i]->relativePosition.y = 10000;
    }
}