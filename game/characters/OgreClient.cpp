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
    : Ogre(), renderX(0), renderY(0), lastSnapshotId(0)
{
}

void OgreClient::clientLoad(LevelClient* _level)
{
    level = _level;

    quadIndex = level->vertexMap_01.requestVertexIndex();
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite("Ogre");
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}

void OgreClient::clientUpdate(sf::Time &gameTime)
{
    // Sprite
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);

    // Position
    bit::VectorMath::incrementTowards(renderX, renderY, Body::schema.x, Body::schema.y, 4, 4);

    float spriteWidth = 84;
    float spriteHeight = 116;
    float xFootOffset = 30;
    float yFootOffset = 20;
    float levelCenterX = renderX + Body::schema.width / 2;
    float levelCenterY = renderY + Body::schema.height / 2;

    sf::Vector2f r = bit::VectorMath::normalToIsometric(levelCenterX, levelCenterY);
    r.x = r.x - spriteWidth / 2 + xFootOffset / 2;
    r.y = r.y - spriteHeight + yFootOffset;

    float z = bit::Math::calculateDrawDepth(r.y + spriteHeight);
    bit::Vertex3* quad = &level->vertexMap_01.vertexArray[quadIndex];
    bit::VertexHelper::positionQuad(quad, r.x, r.y, z, spriteWidth, spriteHeight);

    // Color and luminence
    sf::Color color(255 * Body::schema.illumination, 255 * Body::schema.illumination, 255 * Body::schema.illumination);
    bit::VertexHelper::colorQuad(quad, color);
}

void OgreClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
	Ogre::handleSnapshot(packet, full);

    if(renderX == 0 && renderY == 0)
    {
        renderX = Body::schema.x;
        renderY = Body::schema.y;
    }
}

void OgreClient::reset()
{
    renderX = renderY = 0;
    bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}