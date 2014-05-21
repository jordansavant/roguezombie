#include "OgreClient.hpp"
#include "Ogre.hpp"
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

OgreClient::OgreClient()
    : CharacterClient(), sprite(84, 116, 30, 96)
{
}

void OgreClient::clientLoad(LevelClient* _level)
{
    level = _level;

    sprite.load(this, level->state->rogueZombieGame->spriteLoader, level->vertexMap_01);
    sprite.setBodySprites(std::string("Ogre_Head"), std::string("Ogre_FrontArm"), std::string("Ogre_Body"), std::string("Ogre_Shadow"));
    sprite.setDeathSprite(std::string("Zombie_Dead"));
}

void OgreClient::clientUpdate(sf::Time &gameTime)
{
    sprite.update(gameTime);
}

void OgreClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
	CharacterClient::handleSnapshot(packet, full);

    sprite.syncronizeEquipment();
}

void OgreClient::reset()
{
    sprite.reset();
}