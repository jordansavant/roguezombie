#include "OgreClient.hpp"
#include "Ogre.hpp"
#include "SFML/Graphics.hpp"
#include "../WorldClient.hpp"
#include "../GameplayState.hpp"
#include "../../bitengine/Game.hpp"
#include "../../bitengine/Network.hpp"
#include "../../bitengine/Input.hpp"
#include "../../bitengine/Math.hpp"
#include "../../bitengine/System.hpp"

OgreClient::OgreClient()
    : Ogre(), renderX(0), renderY(0)
{
}

void OgreClient::clientLoad(WorldClient* _world, sf::Texture* texture)
{
    world = _world;
    renderTexture = texture;
    renderSprite.setTexture(*texture);
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
    renderSprite.setPosition(r.x, r.y);
}

void OgreClient::clientDraw(sf::RenderWindow &window, sf::Time &gameTime)
{
    window.draw(renderSprite);
}

void OgreClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
	Ogre::handleSnapshot(packet, full);

    if(full)
    {
        renderX = Body::deltaState.x;
        renderY = Body::deltaState.y;
    }
}