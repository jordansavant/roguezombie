#include "InteractionMenu.hpp"
#include "Hud.hpp"
#include "../ClientRequest.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Network.hpp"
#include "../StateGamePlay.hpp"
#include "../LevelClient.hpp"
#include "../CharacterClient.hpp"
#include "../RogueZombieGame.hpp"
#include "../mission/MissionClient.hpp"
#include "../Interaction.hpp"

InteractionMenu::InteractionMenu(Hud* _hud)
    : bit::Container(-50, -50, 300, 400, bit::Element::AnchorType::BottomRight), hud(_hud)
{
    opacity = 1;
    lambdaListenToInput = std::bind(&Hud::typicalContainerControl, hud, std::placeholders::_1,std::placeholders::_2, std::placeholders::_3);
    canHaveFocus = true;
}

void InteractionMenu::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::Container::update(window, gameTime);
}

void InteractionMenu::handleInteractionTree(bit::ServerPacket &packet, unsigned int tileId)
{
    unsigned int optionSize;
    packet >> optionSize;

    clearChildren();
    if(optionSize > 0)
    {
        int y = 0;
        for(unsigned int i=0; i < optionSize; i++)
        {
            InteractionMenu* m = this;
            Interaction::Type it;
            bit::NetworkHelper::unpackEnum<sf::Uint32, Interaction::Type>(packet, it);
        
            bit::Label* option = new bit::Label(0, y, 0, 0, bit::Element::AnchorType::TopLeft, std::bind(&Hud::typicalElementControl, hud, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            option->setSfFontSize(24);
            option->setSfFont(hud->journalFont);
            option->normalColor = sf::Color::White;
            option->setSfFontString(Interaction::getTitle(it));
            option->canHaveFocus = true;
            option->onActivate = [it, m, tileId] (Element* e){
                    InteractionMenu* mx = m;
                    Interaction::Type itx(it);
                    unsigned int tileIdx = tileId;
                    m->hud->state->serverRequest(
                        [itx, tileIdx, mx] (bit::ClientPacket &packet) {
                            packet << sf::Uint32(ClientRequest::Interaction);
                            packet << sf::Uint32(itx);
                            packet << sf::Uint32(tileIdx);
                        },
                        [itx, tileIdx, mx] (bit::ServerPacket &packet) {
                        }
                    );
            };
            addChild(option);

            y += 30;
        }
    }
}