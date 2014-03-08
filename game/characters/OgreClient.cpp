#include "OgreClient.hpp"
#include "Ogre.hpp"
#include "SFML/Graphics.hpp"
#include "../WorldClient.hpp"
#include "../GameplayState.hpp"
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

void OgreClient::clientLoad(WorldClient* _world)
{
    world = _world;

    quadIndex = world->vertexMap_01.requestVertexIndex();
    sprite = world->state->game->spriteLoader->getSprite("Ogre");
    sprite->applyToQuad(&world->vertexMap_01.vertexArray[quadIndex]);
}

void OgreClient::clientUpdate(sf::Time &gameTime)
{
    bit::VectorMath::incrementTowards(renderX, renderY, Body::deltaState.x, Body::deltaState.y, 4);

    float spriteWidth = 84;
    float spriteHeight = 116;
    float xFootOffset = 30;
    float yFootOffset = 20;
    float worldCenterX = renderX + Body::deltaState.width / 2;
    float worldCenterY = renderY + Body::deltaState.height / 2;

    sf::Vector2f r = bit::VectorMath::normalToIsometric(worldCenterX, worldCenterY);
    r.x = r.x - spriteWidth / 2 + xFootOffset / 2;
    r.y = r.y - spriteHeight + yFootOffset;

    float z = bit::Math::calculateDrawDepth(r.y + spriteHeight);
    bit::Vertex3* quad = &world->vertexMap_01.vertexArray[quadIndex];
    bit::VertexHelper::positionQuad(quad, r.x, r.y, z, spriteWidth, spriteHeight);
}

void OgreClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
	Ogre::handleSnapshot(packet, full);

    if(renderX == 0 && renderY == 0)
    {
        renderX = Body::deltaState.x;
        renderY = Body::deltaState.y;
    }
}

void OgreClient::reset()
{
    bit::VertexHelper::resetQuad(&world->vertexMap_01.vertexArray[quadIndex]);
}