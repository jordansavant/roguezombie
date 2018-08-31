#include "InformationPanel.hpp"
#include "Hud.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "../TileClient.hpp"
#include "../StructureClient.hpp"
#include "../CharacterClient.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/System.hpp"
#include <sstream>

InformationPanel::InformationPanel(Hud* _hud)
    : bit::Container(60, 130, 320, 320, bit::Element::AnchorType::TopLeft, std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)), hud(_hud)
{
    scaleStyle = ScaleStyle::PowerOfTwo;
    managesOpacity = true;
    opacity = Hud::hoverlessOpacity;

    information = new bit::Label(0, 0, 0, 0, bit::Element::AnchorType::TopLeft);
    information->setSfFontSize(Hud::font_noteSize);
    information->setSfFont(hud->journalFont);
    information->normalColor = sf::Color(0, 255, 0);
    information->setSfFontString("");
    addChild(information);
}

void InformationPanel::display(TileClient* tileClient)
{
    // Ok lets pull some information out of the tile client and display it
    std::stringstream ss;

    ss << tileClient->getTitle() << " (" << (int)tileClient->schema.x << "," << (int)tileClient->schema.y << ")\n";

    if (tileClient->hasCharacter)
    {
        CharacterClient* cc = tileClient->characterClient;
        ss << cc->getTitle() << "\n";
        ss << "- Health: " << cc->schema.health << "/" << cc->schema.maxHealth << "\n";
        ss << "- Dexterity: " << cc->schema.dexterity << "\n";
        ss << "- Speed: " << cc->schema.speed << "\n";
        ss << "- Strength: " << cc->schema.strength << "\n";
        if (cc->hasEquipment[Character::EquipmentSlot::WeaponPrimary])
        {
            ItemClient* ic = &cc->equipment[Character::EquipmentSlot::WeaponPrimary];
            ss << "- Weapon: " << Item::getTitle(ic->schema.type) << "\n";
            ss << "  - Range: " << ic->schema.effectiveRangeInTiles << "\n";
            ss << "  - Damage: " << ic->schema.minimumDamage << "-" << ic->schema.maximumDamage << "\n";
        }
        if (cc->activeEffects.size() > 0)
        {
            for (unsigned int i=0; i < cc->activeEffects.size(); i++)
            {
                ss << "* " << CharacterEffect::getAdjective(cc->activeEffects[i].type) << "\n";
                ss << "  - Damage: " << cc->activeEffects[i].damageAmount << "\n";
                ss << "  - Every: " << cc->activeEffects[i].tileInterval << " moves\n";
                ss << "  - For: " << cc->activeEffects[i].tileCounter << "/" << cc->activeEffects[i].maxTiles << " moves\n";
            }
        }
    }
    else if (tileClient->hasStructure)
    {
        ss << tileClient->structureClient->getTitle() << "\n";
    }

    information->setSfFontString(ss.str());
}

void InformationPanel::clear()
{
    // Ok lets pull some information out of the tile client and display it
    information->setSfFontString("");
}