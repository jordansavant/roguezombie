#pragma once
#ifndef RZ_OPTIONS_H
#define RZ_OPTIONS_H

#include "HudMenu.hpp"
#include "SFML/Graphics.hpp"
#include "../../bitengine/Gui.hpp"
#include "../../bitengine/Graphics.hpp"

class Hud;

class Options : public HudMenu
{
public:

    Options(Hud* hud);

    bit::Label* title;
    bit::Label* musicIncrease;
    std::string musicIncreaseText;
    std::string musicIncreaseTextFocus;
    bit::Label* musicDecrease;
    std::string musicDecreaseText;
    std::string musicDecreaseTextFocus;
    bit::Label* quitGame;
    std::string quitGameText;
    std::string quitGameTextFocus;
    unsigned int naviconQuadIndex;
    bit::Sprite* naviconSprite;

    bit::Slider* musicSlider;

    sf::Texture sliderHandleTexture;
    sf::Texture sliderBackgroundTexture;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

    void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void hide();

    virtual void show();

    void configureLabel(bit::Label* label, std::string* text, std::string* focusText);

};

#endif