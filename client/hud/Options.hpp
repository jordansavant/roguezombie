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
    bit::Label* quitGame;
    std::string quitGameText;
    std::string quitGameTextFocus;
    unsigned int naviconQuadIndex;
    bit::Sprite* naviconSprite;

    std::string musicSliderText;
    bit::Slider* musicSlider;

    std::string soundSliderText;
    bit::Slider* soundSlider;

    unsigned int systemResolutionIndex;
    unsigned int selectedResolutionIndex;

    std::string resolutionSliderText;
    bit::Slider* resolutionSlider;
    bit::Label* resolutionDisplayLabel;
    bit::Label* resolutionChangeLabel;
    std::string resolutionChangeText;
    std::string resolutionChangeTextFocus;

    sf::Texture sliderHandleTexture;
    sf::Texture sliderBackgroundTexture;

    virtual void update(sf::RenderWindow &window, sf::Time &gameTime);

    void updateTargets(sf::RenderWindow &window, sf::Time &gameTime);

    void updateReals(sf::RenderWindow &window, sf::Time &gameTime);

    virtual void hide();

    virtual void show();

    void configureLabel(bit::Label* label, std::string* text, std::string* focusText);

    void configureSlider(bit::Slider* slider, std::string* text, float min, float max);

    void syncResolutionOptionWithSystem();

    void changeSelectedResolutionIndex(unsigned int selectedResolution);

    void applyResolution();

    void assignSystemResolutionIndex();

    std::string getCurrentResolutionString();

    std::string getResolutionString(sf::Vector2i &cr);

    std::string getResolutionStringByIndex(unsigned int index);

};

#endif