#include "TurnQueue.hpp"
#include "Hud.hpp"
#include "HudElement.hpp"
#include "../../server/Character.hpp"
#include "../CharacterSprite.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include <sstream>

TurnQueue::TurnQueue(Hud* _hud)
    : bit::Container(-20, -20, 20, 128, bit::Element::AnchorType::BottomRight, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), hud(_hud)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    
    // Fill elements with turn queue items
    for(unsigned int i=0; i < 300; i++)
    {
        HudElement* he = new HudElement(0, 10000, 0, 0, Element::AnchorType::Bottom);
        he->load(hud, std::string("optionbar_character"));
        he->scaleStyle = Element::ScaleStyle::PowerOfTwo;
        addChild(he);
    }

    currentAp = new bit::Label(-80, -12, 0, 0, bit::Element::AnchorType::Bottom);
    currentAp->setSfFontSize(24);
    currentAp->setSfFontString(std::string("1/2 AP"));
    currentAp->setSfFont(hud->journalFont);
    currentAp->normalColor = sf::Color::White;
    currentAp->scaleStyle = Element::ScaleStyle::PowerOfTwo;
    currentAp->opacity = 0;
    addChild(currentAp);

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
    bool first = true;
    for(unsigned int i=0; i < size; i++)
    {
        bool shown;
        packet >> shown;

        if(shown)
        {
            if(first)
            {
                sf::Color c(0, 255, 0);
                unsigned int curAp;
                unsigned int maxAp;
                packet >> maxAp;
                packet >> curAp;
                std::stringstream ss;
                ss << curAp << "/" << maxAp << " AP";
                currentAp->setSfFontString(ss.str());
                currentAp->relativePosition.x = -80;
                currentAp->relativePosition.y = -12;
                currentAp->opacity = 1;
                currentAp->normalColor = c;
            }

            Character::Type type;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Character::Type>(packet, type);

            HudElement* he = static_cast<HudElement*>(childElements[i]);
            he->sprite = hud->state->rogueZombieGame->spriteLoader->getSprite("tq_" + CharacterSprite::getSpriteMoniker(type) + "_inactive");
            he->sprite->applyToQuad(&hud->interfaceVertexMap.vertexArray[he->quadIndex]);
            he->scaleStyle = Element::ScaleStyle::PowerOfTwo;
            he->relativePosition.y = y;
            he->color = first ?  sf::Color(125, 255, 125) : sf::Color(200, 200, 200);

            y -= 40;
            first = false;
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