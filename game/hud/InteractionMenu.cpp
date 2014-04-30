#include "InteractionMenu.hpp"
#include "Hud.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Network.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"
#include "../Interaction.hpp"

InteractionMenu::InteractionMenu(Hud* _hud)
    : bit::Container(0, 50, 500, 200, bit::Element::AnchorType::BottomRight), hud(_hud)
{
    opacity = 1;
    options = new bit::Label(0, 0, 0, 0, bit::Element::AnchorType::TopLeft);
    options->setSfFontSize(24);
    options->setSfFont(hud->journalFont);
    options->normalColor = sf::Color::White;
    addChild(options);
}

void InteractionMenu::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::Container::update(window, gameTime);

    options->opacity = opacity;
}

void InteractionMenu::handleInteractionTree(bit::ServerPacket &packet)
{
    unsigned int optionSize;
    packet >> optionSize;

    std::string s("");
    for(unsigned int i=0; i < optionSize; i++)
    {
        Interaction::Type it;
        bit::NetworkHelper::unpackEnum<sf::Uint32, Interaction::Type>(packet, it);
        s += Interaction::getTitle(it) + "\n";
    }
    options->setSfFontString(s);
}