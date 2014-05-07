#include "CharacterSprite.hpp"
#include "CharacterClient.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Math.hpp"

CharacterSprite::CharacterSprite()
    : renderX(0), renderY(0), vertexMap(NULL), spriteLoader(NULL), headSprite(NULL), frontarmSprite(NULL), bodySprite(NULL), shadowSprite(NULL)
{
}

void CharacterSprite::load(CharacterClient* _character, bit::SpriteLoader* _spriteLoader, bit::VertexMap &_vertexMap)
{
    character = _character;
    spriteLoader = _spriteLoader;
    vertexMap = &_vertexMap;

    // Allocate quads in reverse order for back to front rendered graphics
    // this allows us to use the same Z and properly sort the character vertices
    shadowQuadIndex = vertexMap->requestVertexIndex();
    shadowQuad = &vertexMap->vertexArray[shadowQuadIndex];
    
    bodyQuadIndex = vertexMap->requestVertexIndex();
    bodyQuad = &vertexMap->vertexArray[bodyQuadIndex];
    
    headQuadIndex = vertexMap->requestVertexIndex();
    headQuad = &vertexMap->vertexArray[headQuadIndex];
    
    frontarmQuadIndex = vertexMap->requestVertexIndex();
    frontarmQuad = &vertexMap->vertexArray[frontarmQuadIndex];

    shadowSprite = spriteLoader->getSprite("Zombie_Shadow");
    shadowSprite->applyToQuad(&_vertexMap.vertexArray[shadowQuadIndex]);
    bodySprite = spriteLoader->getSprite("Zombie_Body");
    bodySprite->applyToQuad(&_vertexMap.vertexArray[bodyQuadIndex]);
    headSprite = spriteLoader->getSprite("Zombie_Head");
    headSprite->applyToQuad(&_vertexMap.vertexArray[headQuadIndex]);
    frontarmSprite = spriteLoader->getSprite("Zombie_FrontArm");
    frontarmSprite->applyToQuad(&_vertexMap.vertexArray[frontarmQuadIndex]);
}

void CharacterSprite::update(sf::Time &gameTime)
{
    // Interpolate position
    bit::VectorMath::incrementTowards(renderX, renderY, character->BodyClient::schema.x, character->BodyClient::schema.y, 4, 4);

    // Calculate render position given sprite information and the isometric rendering
    float spriteWidth = 21;
    float spriteHeight = 29;
    float xFootOffset = 8;
    float yFootOffset = 5;
    float levelCenterX = renderX + character->BodyClient::schema.width / 2;
    float levelCenterY = renderY + character->BodyClient::schema.height / 2;
    sf::Vector2f r = bit::VectorMath::normalToIsometric(levelCenterX, levelCenterY);
    r.x = r.x - spriteWidth / 2 + xFootOffset / 2;
    r.y = r.y - spriteHeight + yFootOffset;

    // Position
    float z = bit::Math::calculateDrawDepth(r.y + spriteHeight);
    bit::VertexHelper::positionQuad(headQuad, r.x, r.y, z, spriteWidth, spriteHeight);
    bit::VertexHelper::positionQuad(frontarmQuad, r.x, r.y, z, spriteWidth, spriteHeight);
    bit::VertexHelper::positionQuad(bodyQuad, r.x, r.y, z, spriteWidth, spriteHeight);
    bit::VertexHelper::positionQuad(shadowQuad, r.x, r.y, z, spriteWidth, spriteHeight);

    // Color and luminence
    sf::Color color((int)(255.0f * character->BodyClient::schema.illumination), (int)(255.0f * character->BodyClient::schema.illumination), (int)(255.0f * character->BodyClient::schema.illumination));
    bit::VertexHelper::colorQuad(headQuad, color);
    bit::VertexHelper::colorQuad(frontarmQuad, color);
    bit::VertexHelper::colorQuad(bodyQuad, color);
    bit::VertexHelper::colorQuad(shadowQuad, color);

    // Sprite
    headSprite->applyToQuad(headQuad);
    frontarmSprite->applyToQuad(frontarmQuad);
    bodySprite->applyToQuad(bodyQuad);
    shadowSprite->applyToQuad(shadowQuad);
}

void CharacterSprite::reset()
{
    renderX = renderY = 0;
    bit::VertexHelper::resetQuad(headQuad);
    bit::VertexHelper::resetQuad(frontarmQuad);
    bit::VertexHelper::resetQuad(bodyQuad);
    bit::VertexHelper::resetQuad(shadowQuad);
}