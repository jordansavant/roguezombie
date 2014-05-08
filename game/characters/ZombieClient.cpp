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
    : CharacterClient(), sprite(25, 37, 10, 31)
{
}

void ZombieClient::clientLoad(LevelClient* _level)
{
    level = _level;

    sprite.load(this, level->state->rogueZombieGame->spriteLoader, level->vertexMap_01);
    sprite.setSprites(std::string("Zombie_Head"), std::string("Zombie_FrontArm"), std::string("Zombie_Body"), std::string("Zombie_Shadow"));
}

void ZombieClient::clientUpdate(sf::Time &gameTime)
{
    sprite.update(gameTime);
}

void ZombieClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
	CharacterClient::handleSnapshot(packet, full);

    if(sprite.renderX == 0 && sprite.renderY == 0)
    {
        sprite.renderX = BodyClient::schema.x;
        sprite.renderY = BodyClient::schema.y;
    }
    sprite.syncronizeEquipment();
}

void ZombieClient::reset()
{
    sprite.reset();
}