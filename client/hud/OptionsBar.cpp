#include "OptionsBar.hpp"
#include "Journal.hpp"
#include "Hud.hpp"
#include "HudElement.hpp"
#include "../../bitengine/Input.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../RogueZombieGame.hpp"

OptionsBar::OptionsBar(Hud* _hud)
    : bit::Container(50, 50, 320, 64, bit::Element::AnchorType::TopLeft, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3)), hud(_hud)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    managesOpacity = true;
    opacity = Hud::hoverlessOpacity;

    makeHoverable(hud->state->rogueZombieGame->inputManager,
        [](bit::Hoverable* h, bit::Element* e){
            //e->immediateEffect(new bit::FadeEffect(100, 1));
        },
        [](bit::Hoverable* h, bit::Element* e){
            //e->immediateEffect(new bit::FadeEffect(100, Hud::hoverlessOpacity));
        }
    );
    
    int originX = 0;
    int xpadding = 2;
    //primary = new HudElement(originX, 0, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    //primary->load(hud, std::string("optionbar_primary"));
    //primary->onActivate = [_hud] (Element* e) {
    //    if(_hud->state->mode != StateGamePlay::Mode::Free)
    //        _hud->state->changeMode(StateGamePlay::Mode::Free);
    //    else
    //        _hud->state->requestStackPop();
    //};
    //addChild(primary);
    //originX += primary->sprite->width + xpadding;

    options = new HudElement(originX, 0, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    options->load(hud, std::string("optionbar_options"));
    options->onActivate = [_hud] (Element* e) {
        if(_hud->state->mode == StateGamePlay::Mode::Options)
            _hud->state->changeMode(StateGamePlay::Mode::Free);
        else
            _hud->state->changeMode(StateGamePlay::Mode::Options);
    };
    addChild(options);
    originX += options->sprite->width + xpadding;

    inventory = new HudElement(originX, 0, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    inventory->load(hud, std::string("optionbar_inventory"));
    inventory->onActivate = [_hud] (Element* e) {
        if(_hud->state->mode == StateGamePlay::Mode::Inventory || _hud->state->mode == StateGamePlay::Mode::Loot)
            _hud->state->changeMode(StateGamePlay::Mode::Free);
        else
            _hud->state->changeMode(StateGamePlay::Mode::Inventory);
    };
    addChild(inventory);
    originX += inventory->sprite->width + xpadding;

    journal = new HudElement(originX, 0, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    journal->load(hud, std::string("optionbar_journal"));
    journal->onActivate = [_hud] (Element* e) {
        if(_hud->state->mode == StateGamePlay::Mode::Journal)
            _hud->state->changeMode(StateGamePlay::Mode::Free);
        else
            _hud->state->changeMode(StateGamePlay::Mode::Journal);
    };
    addChild(journal);
    originX += journal->sprite->width + xpadding;

    character = new HudElement(originX, 0, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    character->load(hud, std::string("optionbar_character"));
    addChild(character);
    originX += character->sprite->width + xpadding;

    map = new HudElement(originX, 0, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    map->load(hud, std::string("optionbar_map"));
    map->onActivate = [_hud] (Element* e) {
        _hud->toggleMap();
    };
    addChild(map);
    originX += map->sprite->width + xpadding;

    targetWidth = originX;
    targetHeight = options->sprite->height;
}