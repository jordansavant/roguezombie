#include "ZombieClient.hpp"
#include "Zombie.hpp"
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

ZombieClient::ZombieClient()
    : CharacterClient()
{
    sprite.width = 40;
    sprite.height = 40;
    sprite.baseOffsetX = 20;
    sprite.baseOffsetY = 35;
}

void ZombieClient::clientLoad(LevelClient* _level)
{
    level = _level;

    sprite.load(this, level->state->rogueZombieGame->spriteLoader, level->vertexMap_01);
}

void ZombieClient::clientUpdate(sf::Time &gameTime)
{
    sprite.update(gameTime);
}

void ZombieClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
	CharacterClient::handleSnapshot(packet, full);

    sprite.syncronizeEquipment();
}

void ZombieClient::reset()
{
    sprite.reset();
}