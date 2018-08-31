#pragma once
#ifndef RZ_INFORMATIONPANEL_H
#define RZ_INFORMATIONPANEL_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"

class HudElement;
class Hud;
class TileClient;

class InformationPanel : public bit::Container
{
public:

    InformationPanel(Hud* hud);

    Hud* hud;
    bit::Label* information;

    void display(TileClient* tileClient);

    void clear();
};

#endif