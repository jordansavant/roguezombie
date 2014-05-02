#include "Console.hpp"
#include "Hud.hpp"
#include "../../bitengine/Input.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"

Console::Console(Hud* _hud)
    : bit::Container(50, -50, 500, 200, bit::Element::AnchorType::BottomLeft, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), hud(_hud)
{
}

void Console::print(std::string message)
{
    // Create the new line
    bit::Label* line = new bit::Label(-400, 0, 0, 0, bit::Element::AnchorType::BottomLeft);
    line->setSfFontSize(24);
    line->setSfFont(hud->journalFont);
    line->normalColor = sf::Color::White;
    line->setSfFontString(message);
    
    // Add the child
    addChild(line);

    // Queue the thing for its own removal;
    Console* console = this;
    line->queueEffect(new bit::MoveEffect(500, bit::Easing::OutQuint, 400, 0))->queueEffect(new bit::Effect(2000))->queueEffect(new bit::FadeEffect(500, 0, bit::Easing::Linear, [console] (bit::Element* e, bit::Effect* f){
        e->removeFromParent = true;
    }));

    // Update line positions
    float y = 0;
    for(unsigned int i=childElements.size(); i > 0; i--)
    {
        unsigned int index = i-1;
        childElements[index]->relativePosition.y = y;
        y -= 30;
    }
}