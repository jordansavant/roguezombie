#pragma once
#ifndef RZ_DEVTERMINAL_H
#define RZ_DEVTERMINAL_H

#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"
#include "Frame.hpp"

class Hud;

class DevTerminal : public Frame
{
public:

	DevTerminal(Hud* hud);

	bit::Label* input;
	bit::InputTextListener* inputTextListener;

	virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

	virtual void hide();

	virtual void show();

};

#endif