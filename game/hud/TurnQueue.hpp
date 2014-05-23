#pragma once
#ifndef RZ_TURNQUEUE_H
#define RZ_TURNQUEUE_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"

class HudElement;
class Hud;

class TurnQueue : public bit::Container
{
public:

    TurnQueue(Hud* hud);

    Hud* hud;

    void handleTurnQueue(bit::ServerPacket &packet);
};

#endif