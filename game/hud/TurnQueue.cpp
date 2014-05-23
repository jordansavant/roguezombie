#include "TurnQueue.hpp"
#include "Hud.hpp"
#include "HudElement.hpp"

TurnQueue::TurnQueue(Hud* _hud)
    : bit::Container(-50, -50, 20, 128, bit::Element::AnchorType::BottomRight, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), hud(_hud)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    
    // Fill elements with turn queue items
    for(unsigned int i=0; i < 20; i++)
    {
        HudElement* he = new HudElement(0, 0, 0, 0, Element::AnchorType::Bottom);
        he->load(hud, std::string("optionbar_character"));
        addChild(he);
    }

    //targetWidth = originX;
    //targetHeight = primary->sprite->height;
}

void TurnQueue::handleTurnQueue(bit::ServerPacket &packet)
{
}