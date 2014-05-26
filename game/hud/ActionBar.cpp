#include "ActionBar.hpp"
#include "Hud.hpp"
#include "HudElement.hpp"
#include "../Command.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"

ActionBar::ActionBar(Hud* hud)
    : Frame(hud, 0, 200, 1100, 200, bit::Element::AnchorType::Bottom, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3))
{
    scaleStyle = bit::Element::ScaleStyle::PowerOfTwo;

    int originX = 20;
    int originY = 10;
    int xpadding = 2;

    wait = new HudElement(originX, originY, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    wait->load(hud, std::string("optionbar_hourglass"));
    wait->onActivate = [hud] (Element* e) {
        // Skip Command
        hud->displayMessage(std::string("Player skips action"));
        Command cmd;
        cmd.type = Command::Type::CombatCommand;
        cmd.pack = [] (sf::Packet &packet) {
            packet << sf::Uint32(Command::Target::NoTarget);
            packet << sf::Uint32(Command::NonTargetCommand::Skip);
        };
        hud->state->issueCommand(cmd);
    };
    wait->makeHoverable(hud->state->rogueZombieGame->inputManager, [hud](bit::Hoverable* h, bit::Element* me) {
        hud->displayTooltipAt(std::string("Skip...\nGo to next action"), me->left + me->width / 2, me->top - 5);
    });
    addChild(wait);
    originX += wait->sprite->width + xpadding;

    attack = new HudElement(originX, originY, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    attack->load(hud, std::string("optionbar_options"));
    attack->onActivate = [hud] (Element* e) {
        // Attack Command
        Command cmd;
        cmd.type = Command::Type::CombatCommand;
        cmd.pack = [] (sf::Packet &packet) {
            packet << sf::Uint32(Command::Target::Character);
            packet << sf::Uint32(Command::TargetCharacterCommand::Attack);
        };
        hud->state->issueCommand(cmd);
    };
    attack->makeHoverable(hud->state->rogueZombieGame->inputManager, [hud](bit::Hoverable* h, bit::Element* me) {
        hud->displayTooltipAt(std::string("Attack...\nMust target enemy"), me->left + me->width / 2, me->top - 5);
    });
    addChild(attack);
    originX += attack->sprite->width + xpadding;
}

void ActionBar::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    Frame::update(window, gameTime);
}

void ActionBar::hide()
{
    canHaveFocus = false;
    clearEffects();
    relativePosition.y = 120;
    immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, 0, targetHeight - 120));
    immediateEffect(new bit::FadeEffect(150, 0));
}

void ActionBar::show()
{
    canHaveFocus = true;
    clearEffects();
    relativePosition.y = targetHeight;
    immediateEffect(new bit::MoveEffect(300, bit::Easing::OutQuart, 0, -targetHeight + 120));
    immediateEffect(new bit::FadeEffect(300, 1));
}
