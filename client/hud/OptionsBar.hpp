#pragma once
#ifndef RZ_OPTIONSBAR_H
#define RZ_OPTIONSBAR_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"

class HudElement;
class Hud;

class OptionsBar : public bit::Container
{
public:

    OptionsBar(Hud* hud);

    Hud* hud;
    //HudElement* primary;
    HudElement* options;
    //HudElement* journal;
    //HudElement* character;
    HudElement* inventory;
    HudElement* map;
};

#endif