#include "Options.hpp"
#include "Hud.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Audio.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../RogueZombieGame.hpp"

Options::Options(Hud* _hud)
    : HudMenu(_hud, 0, 0, 691, 728, bit::Element::AnchorType::Right, true)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    managesOpacity = true;

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

    musicIncrease = new bit::Label(initX, initY, 0, 0, bit::Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    musicIncreaseText = std::string("MUSIC UP");
    musicIncreaseTextFocus = musicIncreaseText + " <<";
    configureLabel(musicIncrease, &musicIncreaseText, &musicIncreaseTextFocus);
    musicIncrease->onActivate = [_hud] (bit::Element* e) {
        _hud->state->rogueZombieGame->musicManager->changeVolume(10);
    };
    addChild(musicIncrease);

    initY += ySeparation;

    musicDecrease = new bit::Label(initX, initY, 0, 0, bit::Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    musicDecreaseText = std::string("MUSIC DOWN");
    musicDecreaseTextFocus = musicDecreaseText + " <<";
    configureLabel(musicDecrease, &musicDecreaseText, &musicDecreaseTextFocus);
    musicDecrease->onActivate = [_hud] (bit::Element* e) {
        _hud->state->rogueZombieGame->musicManager->changeVolume(-10);
    };
    addChild(musicDecrease);

    initY += ySeparation;

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
