#pragma once
#ifndef BIT_FRAMETIMER_H
#define BIT_FRAMETIMER_H

#include "SFML/Graphics.hpp"

namespace bit
{
    class FrameTimer
    {
    public:

        //FrameTimer();

        FrameTimer(std::string &fontPath, int x, int y);

        sf::Font fpsFont;
        sf::Text fpsText;
		sf::Time fpsUpdateMicro;
		sf::Time fpsDrawMicro;
        int x, y;

        void update(sf::Time &gameTime);

		void draw(sf::RenderWindow &window, sf::Time &gameTime);

    private:

        bool loaded;
    };
}

#endif