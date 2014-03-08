#include "FrameTimer.hpp"
#include <string>
#include <sstream>

//bit::FrameTimer::FrameTimer()
//{
//}

bit::FrameTimer::FrameTimer(std::string &fontPath, int _x, int _y)
{
    loaded = fpsFont.loadFromFile(fontPath);
    if(loaded)
    {
        fpsText.setFont(fpsFont);
        fpsText.setCharacterSize(16);
        x = _x;
        y = _y;
        fpsText.setPosition(x, y);
    }
}

void bit::FrameTimer::update(sf::Time &gameTime)
{
    if(loaded)
    {
        fpsText.setPosition(x, y);
	    fpsUpdateMicro = gameTime;
    }
}

void bit::FrameTimer::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(loaded)
    {
        fpsDrawMicro = gameTime;
	    std::stringstream t  (std::stringstream::in | std::stringstream::out);
	    int u = 1 / fpsUpdateMicro.asSeconds();
	    int d = 1 / fpsDrawMicro.asSeconds();
	    t << "FPS U: " << u << " / D: " << d;
	    fpsText.setString(t.str());

	    window.draw(fpsText);
    }
}