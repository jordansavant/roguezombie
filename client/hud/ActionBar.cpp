#include "ActionBar.hpp"
#include "ActionBarSlot.hpp"
#include "Hud.hpp"
#include "HudElement.hpp"
#include "../../server/Command.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"

ActionBar::ActionBar(Hud* hud)
    : Frame(hud, 0, 200, 950, 200, bit::Element::AnchorType::Bottom, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3))
{
    scaleStyle = bit::Element::ScaleStyle::PowerOfTwo;
    managesOpacity = true;
    opacity = 1;

    int originX = 20;
    int originY = 10;
    int xpadding = 2;

    attack = new HudElement(originX, originY, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    attack->load(hud, std::string("optionbar_crosshair"));
    attack->onActivate = [hud] (Element* e) {
        // Attack Command
        if(hud->state->target.active())
        {
            Hud* hudx = hud;
            Command cmd;
            cmd.type = Command::Type::CombatCommand;
            cmd.pack = [hudx] (sf::Packet &packet) {
                packet << sf::Uint32(Command::Target::Character);
                packet << sf::Uint32(hudx->state->target.tileId);
                packet << sf::Uint32(Command::TargetCharacterCommand::Attack);
            };
            hud->state->issueCommand(cmd);
        }
        else
        {
            hud->displayMessage(std::string("No target to attack"));
        }
    };
    //attack->makeHoverable(hud->state->rogueZombieGame->inputManager, [hud](bit::Hoverable* h, bit::Element* me) {
    //    hud->displayTooltipAt(std::string("Attack"), me->left + me->width / 2, me->top - 10, 1, 0);
    //}, [hud](bit::Hoverable* h, bit::Element* me) {
    //    hud->hideTooltip();
    //});
    addChild(attack);
    originX += attack->sprite->width + xpadding;
    
    wait = new HudElement(originX, originY, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    wait->load(hud, std::string("optionbar_hourglass"));
    wait->onActivate = [hud] (Element* e) {
        // Skip Command
        Command cmd;
        cmd.type = Command::Type::CombatCommand;
        cmd.pack = [] (sf::Packet &packet) {
            packet << sf::Uint32(Command::Target::NoTarget);
            packet << sf::Uint32(Command::NonTargetCommand::Skip);
        };
        hud->displayMessage(std::string("Player skips action"));
        hud->state->issueCommand(cmd);
    };
    //wait->makeHoverable(hud->state->rogueZombieGame->inputManager, [hud](bit::Hoverable* h, bit::Element* me) {
    //    hud->displayTooltipAt(std::string("Skip Action"), me->left + me->width / 2, me->top - 10, 1, 0);
    //}, [hud](bit::Hoverable* h, bit::Element* me) {
    //    hud->hideTooltip();
    //});
    addChild(wait);
    originX += wait->sprite->width + xpadding;
    
    swap = new HudElement(originX, originY, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    swap->load(hud, std::string("optionbar_swap"));
    swap->onActivate = [hud] (Element* e) {
        // Attack Command
        Hud* hudx = hud;
        Command cmd;
        cmd.type = Command::Type::CombatCommand;
        cmd.pack = [hudx] (sf::Packet &packet) {
            packet << sf::Uint32(Command::Target::NoTarget);
            packet << sf::Uint32(Command::NonTargetCommand::SwapWeapon);
        };
        hud->displayMessage(std::string("Player swaps weapon"));
        hud->state->issueCommand(cmd);
    };
    //swap->makeHoverable(hud->state->rogueZombieGame->inputManager, [hud](bit::Hoverable* h, bit::Element* me) {
    //    hud->displayTooltipAt(std::string("Swap Weapons"), me->left + me->width / 2, me->top - 10, 1, 0);
    //}, [hud](bit::Hoverable* h, bit::Element* me) {
    //    hud->hideTooltip();
    //});
    addChild(swap);
    originX += swap->sprite->width + xpadding;


    // SLOTS
    int y = 10;
    int width = 64;
    int pad = 8;
    int x = originX + pad;
    for(unsigned int j=0; j < 10; j++)
    {
        ActionBarSlot* slot = new ActionBarSlot(hud, x, y, width, width, bit::Element::AnchorType::TopLeft);
        addChild(slot);
        slots.push_back(slot);
        x += width + pad;
    }
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
