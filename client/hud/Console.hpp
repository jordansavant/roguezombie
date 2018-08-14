#pragma once
#ifndef RZ_CONSOLE_H
#define RZ_CONSOLE_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"

class Hud;

class Console : public bit::Container
{
public:

    Console(Hud* hud);

    Hud* hud;

    virtual void print(std::string const&message);

};

#endif