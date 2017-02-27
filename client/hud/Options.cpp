#include "Options.hpp"
#include "Hud.hpp"
#include "../../ResourcePath.h"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Audio.hpp"
#include "../../bitengine/System.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../RZConfig.hpp"

Options::Options(Hud* _hud)
    : HudMenu(_hud, 0, 0, 691, 728, bit::Element::AnchorType::Right, true)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    managesOpacity = true;

    sliderHandleTexture.loadFromFile(resourcePath() + "options_sliderhandle.png");
    sliderBackgroundTexture.loadFromFile(resourcePath() + "options_sliderbg.png");

    checkboxCheckedTexture.loadFromFile(resourcePath() + "options_checkbox_checked.png");
    checkboxUncheckedTexture.loadFromFile(resourcePath() + "options_checkbox_unchecked.png");

    float initX = 38;
    float initY = 30;
    float ySeparation = 40;

    title = new bit::Label(initX, initY, 0, 0, bit::Element::AnchorType::TopLeft);
    title->setSfFontSize(Hud::font_primarySize);
    title->setSfFont(hud->journalFont);
    title->normalColor = sf::Color::Black;
    title->scaleStyle = ScaleStyle::PowerOfTwo;
    addChild(title);

    initY += 60;

    // MUSIC
    musicSlider = new bit::Slider(sliderHandleTexture, sliderBackgroundTexture, initX, initY, 0, 0, Element::AnchorType::TopLeft, [_hud] (bit::Slider* slider, float ratio) {
        _hud->state->rogueZombieGame->musicManager->setByRatio(ratio);
    });
    musicSliderText = "MUSIC";
    configureSlider(musicSlider, &musicSliderText, 0, hud->state->rogueZombieGame->musicManager->getMaximum());
    musicSlider->setFromSystem = [_hud] () {
        return _hud->state->rogueZombieGame->musicManager->getMasterVolume() / _hud->state->rogueZombieGame->musicManager->getMaximum();
    };
    addChild(musicSlider);

    initY += ySeparation;

    // SOUND
    soundSlider = new bit::Slider(sliderHandleTexture, sliderBackgroundTexture, initX, initY, 0, 0, Element::AnchorType::TopLeft, [_hud] (bit::Slider* slider, float ratio) {
        _hud->state->rogueZombieGame->soundManager->setByRatio(ratio);
    });
    soundSliderText = "SOUND";
    configureSlider(soundSlider, &soundSliderText, 0, hud->state->rogueZombieGame->soundManager->getMaximum());
    soundSlider->setFromSystem = [_hud] () {
        return _hud->state->rogueZombieGame->soundManager->getMasterVolume() / _hud->state->rogueZombieGame->soundManager->getMaximum();
    };
    addChild(soundSlider);

    initY += ySeparation;

    // RESOLUTION
    Options* self = this;
    resolutionSlider = new bit::Slider(sliderHandleTexture, sliderBackgroundTexture, initX, initY, 0, 0, Element::AnchorType::TopLeft, [self] (bit::Slider* slider, float ratio) {
        // Change the selected resolution
        unsigned int rounded = bit::Math::round(ratio * slider->max);
        self->changeSelectedResolutionIndex(rounded);
    });
    resolutionSliderText = "D_RES";
    configureSlider(resolutionSlider, &resolutionSliderText, 0, RZConfig::supportedResolutions.size() - 1);
    addChild(resolutionSlider);

    initY += ySeparation;

    // CURRENT RESOLUTION
    resolutionDisplayLabel = new bit::Label(initX + 125, initY, 0, 0, bit::Element::AnchorType::TopLeft);
    resolutionDisplayLabel->setSfFontSize(Hud::font_largeSize);
    resolutionDisplayLabel->setSfFontString(getCurrentResolutionString());
    resolutionDisplayLabel->setSfFont(hud->journalFont);
    resolutionDisplayLabel->normalColor = sf::Color::Black;
    resolutionDisplayLabel->scaleStyle = ScaleStyle::PowerOfTwo;
    addChild(resolutionDisplayLabel);

    // RESOLUTION APPLY
    resolutionChangeLabel = new bit::Label(initX + 320, initY, 0, 0, bit::Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    resolutionChangeText = std::string("APPLY");
    resolutionChangeTextFocus = resolutionChangeText + " <<";
    configureLabel(resolutionChangeLabel, &resolutionChangeText, &resolutionChangeTextFocus);
    resolutionChangeLabel->onActivate = [self] (bit::Element* e) {
        self->applyResolution();
    };
    addChild(resolutionChangeLabel);

    // Update selected resolution to the current resolution
    syncResolutionOptionWithSystem();

    initY += ySeparation;

    // FULLSCREEN
    fullscreenCheckbox = new bit::CheckBox(checkboxCheckedTexture, checkboxUncheckedTexture, initX, initY, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    fullscreenCheckbox->onActivate = [_hud] (bit::Element* e) {
        bit::CheckBox* c = static_cast<bit::CheckBox*>(e);
        c->isChecked = !c->isChecked;
        _hud->state->rogueZombieGame->changeFullscreen(c->isChecked);
    };
    fullscreenCheckbox->setSfFontString(std::string("FULLSCREEN"));
    fullscreenCheckbox->isChecked = hud->state->rogueZombieGame->isFullscreen;
    configureCheckbox(fullscreenCheckbox);
    addChild(fullscreenCheckbox);

    initY += ySeparation;

    // QUIT
    quitGame = new bit::Label(initX, initY, 0, 0, bit::Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    quitGameText = std::string("EXIT GAME");
    quitGameTextFocus = quitGameText + " <<";
    configureLabel(quitGame, &quitGameText, &quitGameTextFocus);
    quitGame->onActivate = [_hud] (bit::Element* e) {
        _hud->state->endGame(StateGamePlay::EndGameReason::Quit);
    };
    addChild(quitGame);


    // Icon
    naviconSprite = hud->state->rogueZombieGame->spriteLoader->getSprite("OptionsNavicon");
    naviconQuadIndex = hud->interfaceVertexMap.requestVertexIndex();
}

void Options::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    HudMenu::update(window, gameTime);

    std::string entry;
    if(elementScale == 1)
        entry = std::string("OPTIONS:\n-----------------------------------------");
    else
        entry = std::string("OPTIONS:\n--------------------------------------------");
    title->setSfFontString(entry);
    title->opacity = opacity;
}

void Options::updateTargets(sf::RenderWindow &window, sf::Time &gameTime)
{
    HudMenu::updateTargets(window, gameTime);

    // Navicon
    bit::Vertex3* naviconQuad = &hud->interfaceVertexMap.vertexArray[naviconQuadIndex];
    bit::VertexHelper::colorQuad(naviconQuad, sf::Color(255, 255, 255, opacity * 255));
}

void Options::updateReals(sf::RenderWindow &window, sf::Time &gameTime)
{
    HudMenu::updateReals(window, gameTime);

    // Navicon
    float naviconZ = Hud::getDrawDepth(Hud::zindex_frames);
    bit::Vertex3* naviconQuad = &hud->interfaceVertexMap.vertexArray[naviconQuadIndex];
    naviconSprite->applyToQuad(naviconQuad);
    bit::VertexHelper::positionQuad(naviconQuad, left, top - naviconSprite->height * elementScale, naviconZ, naviconSprite->width, naviconSprite->height, elementScale);
}

void Options::hide()
{
    canHaveFocus = false;
    isShown = false;
    clearEffects();
    relativePosition.x = 0;
    immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, targetWidth, 0));
    immediateEffect(new bit::FadeEffect(150, 0));
}

void Options::show()
{
    canHaveFocus = true;
    isShown = true;
    clearEffects();
    relativePosition.x = targetWidth;
    immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, -targetWidth, 0));
    immediateEffect(new bit::FadeEffect(300, Hud::popupOpacity));

    // Update the known current resolution to the system's resolution
    syncResolutionOptionWithSystem();
    fullscreenCheckbox->isChecked = hud->state->rogueZombieGame->isFullscreen;
}

void Options::configureLabel(bit::Label* label, std::string* text, std::string* focusText)
{
    label->setSfFontSize(Hud::font_largeSize);
    label->setSfFont(hud->journalFont);
    label->normalColor = sf::Color::Black;
    label->focusedColor = sf::Color::Black;
    label->paddingTop = 0;
    label->paddingLeft = 0;
    label->paddingRight = 5;
    label->paddingBottom = 15;
    label->scaleStyle = ScaleStyle::PowerOfTwo;
    label->setSfFontString((*text));
    label->makeHoverable(hud->state->rogueZombieGame->inputManager,
        [focusText] (bit::Hoverable* h, bit::Element* e)
        {
            bit::Label* l = static_cast<bit::Label*>(e);
            l->setSfFontString(*focusText);
        },
        [text] (bit::Hoverable* h, bit::Element* e)
        {
            bit::Label* l = static_cast<bit::Label*>(e);
            l->setSfFontString(*text);
        }
    );
}

void Options::configureSlider(bit::Slider* slider, std::string* text, float min, float max)
{
    Hud* _hud = hud;
    slider->lambdaOnSliderIsPressed = [_hud] () -> bool {
        return _hud->state->rogueZombieGame->inputManager->isButtonPressed(sf::Mouse::Left);
    };
    slider->lambdaOnSliderIsReleased = [_hud] () -> bool {
        return _hud->state->rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left);
    };
    slider->canHaveFocus = true;
    slider->setSfFontSize(Hud::font_largeSize);
    slider->setSfFont(hud->journalFont);
    slider->normalColor = sf::Color::Black;
    slider->focusedColor = sf::Color::Black;
    slider->scaleStyle = ScaleStyle::PowerOfTwo;
    slider->setSfFontString(*text);
    slider->paddingTop = 0;
    slider->paddingLeft = 0;
    slider->paddingRight = 5;
    slider->paddingBottom = 15;
    slider->textureOffsetX = 0;
    slider->textureOffsetY = 6;
    slider->min = min;
    slider->max = max;
}

void Options::configureCheckbox(bit::CheckBox* checkbox)
{
    Hud* _hud = hud;
    checkbox->canHaveFocus = true;
    checkbox->setSfFontSize(Hud::font_largeSize);
    checkbox->setSfFont(hud->journalFont);
    checkbox->normalColor = sf::Color::Black;
    checkbox->focusedColor = sf::Color::Black;
    checkbox->scaleStyle = ScaleStyle::PowerOfTwo;
    checkbox->paddingTop = 0;
    checkbox->paddingLeft = 0;
    checkbox->paddingRight = 5;
    checkbox->paddingBottom = 15;
    checkbox->textureOffsetX = 15;
    checkbox->textureOffsetY = 7;
}

void Options::syncResolutionOptionWithSystem()
{
    // Get current system resolution
    assignSystemResolutionIndex();

    // Set the selected resolution to the system resolution
    changeSelectedResolutionIndex(systemResolutionIndex);

    // Update the slider position to match
    resolutionSlider->current = (float)selectedResolutionIndex / ((float)RZConfig::supportedResolutions.size() - 1);
}

void Options::changeSelectedResolutionIndex(unsigned int newResolutionIndex)
{
    // Set the new selected resolution
    selectedResolutionIndex = newResolutionIndex;

    // Update the display text to show selected resolution
    resolutionDisplayLabel->setSfFontString(getResolutionStringByIndex(selectedResolutionIndex));
}

void Options::applyResolution()
{
    // Make the system call to change the resolution to what we have selected
    if(selectedResolutionIndex != systemResolutionIndex && selectedResolutionIndex < RZConfig::supportedResolutions.size())
    {
        sf::Vector2i selectedResolution = RZConfig::supportedResolutions[selectedResolutionIndex];
        hud->state->rogueZombieGame->changeResolution(selectedResolution.x, selectedResolution.y);
        assignSystemResolutionIndex();
    }
}

void Options::assignSystemResolutionIndex()
{
    sf::Vector2i cr = hud->state->rogueZombieGame->currentResolution;
    for(unsigned int i=0; i < RZConfig::supportedResolutions.size(); i++)
    {
        if(RZConfig::supportedResolutions[i].x == cr.x && RZConfig::supportedResolutions[i].y == cr.y)
        {
            systemResolutionIndex = i;
            return;
        }
    }

    systemResolutionIndex = RZConfig::defaultResolutionIndex;
}

std::string Options::getCurrentResolutionString()
{
    return getResolutionString(hud->state->rogueZombieGame->currentResolution);
}

std::string Options::getResolutionStringByIndex(unsigned int index)
{
    return getResolutionString(RZConfig::supportedResolutions[index]);
}

std::string Options::getResolutionString(sf::Vector2i &cr)
{
    std::stringstream ss;
    ss << cr.x << "x" << cr.y;
    return ss.str();
}