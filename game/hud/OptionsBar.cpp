#include "OptionsBar.hpp"
#include "Hud.hpp"
#include "HudElement.hpp"
#include "../../bitengine/Input.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../RogueZombieGame.hpp"

OptionsBar::OptionsBar(Hud* _hud)
    : bit::Container(50, 50, _hud->state->rogueZombieGame->targetResolution.x, _hud->state->rogueZombieGame->targetResolution.y, bit::Element::AnchorType::TopLeft), hud(_hud)
{
    lambdaListenToInput = std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3);
    canHaveFocus = true;
    isInfocus = true;

    options = new HudElement(0, 0, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    options->load(hud, std::string("optionbar_options"));
    addChild(options);

    journal = new HudElement(64, 0, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    journal->load(hud, std::string("optionbar_journal"));
    addChild(journal);

    character = new HudElement(128, 0, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    character->load(hud, std::string("optionbar_character"));
    addChild(character);

    inventory = new HudElement(192, 0, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    inventory->load(hud, std::string("optionbar_inventory"));
    addChild(inventory);

    map = new HudElement(256, 0, 0, 0, Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3));
    map->load(hud, std::string("optionbar_map"));
    addChild(map);
}