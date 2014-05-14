#include "HunterClient.hpp"
#include "Hunter.hpp"
#include "SFML/Graphics.hpp"
#include "../LevelClient.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Graphics.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/System.hpp"

HunterClient::HunterClient()
    : CharacterClient(), sprite(40, 40, 20, 35)
{
}

void HunterClient::clientLoad(LevelClient* _level)
{
    level = _level;

    sprite.load(this, level->state->rogueZombieGame->spriteLoader, level->vertexMap_01);
    sprite.setBodySprites(std::string("Hunter_Head"), std::string("Hunter_FrontArm"), std::string("Hunter_Body"), std::string("Hunter_Shadow"));
}

void HunterClient::clientUpdate(sf::Time &gameTime)
{
    sprite.update(gameTime);
}

void HunterClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
	CharacterClient::handleSnapshot(packet, full);

    sprite.syncronizeEquipment();
}

void HunterClient::reset()
{
    sprite.reset();
}