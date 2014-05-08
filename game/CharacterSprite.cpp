#include "CharacterSprite.hpp"
#include "CharacterClient.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Math.hpp"

CharacterSprite::CharacterSprite(unsigned int width, unsigned int height, unsigned int baseOffsetX, unsigned int baseOffsetY)
    : renderX(0), renderY(0), width(width), height(height), baseOffsetX(baseOffsetX), baseOffsetY(baseOffsetY), facingRight(true), lastRenderX(0), lastRenderY(0),
      vertexMap(NULL), spriteLoader(NULL), headSprite(NULL), frontarmSprite(NULL), bodySprite(NULL), shadowSprite(NULL),
      equipmentHeadSprite(NULL), equipmentHeadItemType(Item::Type::None)
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
    
    equipmentHeadQuadIndex = vertexMap->requestVertexIndex();
    equipmentHeadQuad = &vertexMap->vertexArray[equipmentHeadQuadIndex];
}

void CharacterSprite::update(sf::Time &gameTime)
{
    // Interpolate rendering position
    lastRenderX = renderX;
    lastRenderY = renderY;
    if(renderX == 0 && renderY == 0)
    {
        renderX = character->BodyClient::schema.x;
        renderY = character->BodyClient::schema.y;
        lastRenderX = renderX;
        lastRenderY = renderY;
    }
    else
    {
        bit::VectorMath::incrementTowards(renderX, renderY, character->BodyClient::schema.x, character->BodyClient::schema.y, 4, 4);
    }
    if(lastRenderX != renderX || lastRenderY != renderY)
    {
        facingRight = lastRenderX < renderX || lastRenderY > renderY;
    }

    // Calculate render position given sprite information and the isometric rendering
    float spriteWidth = (float)width;
    float spriteHeight = (float)height;
    float xFootOffset = (float)baseOffsetX;
    float yFootOffset = (float)baseOffsetY;
    float levelCenterX = renderX + character->BodyClient::schema.width / 2;
    float levelCenterY = renderY + character->BodyClient::schema.height / 2;
    sf::Vector2f r = bit::VectorMath::normalToIsometric(levelCenterX, levelCenterY);
    r.x -= spriteWidth / 2 + xFootOffset / 2;
    r.y -= yFootOffset;
    float z = bit::Math::calculateDrawDepth(r.y + spriteHeight);
    sf::Color color((int)(255.0f * character->BodyClient::schema.illumination), (int)(255.0f * character->BodyClient::schema.illumination), (int)(255.0f * character->BodyClient::schema.illumination));

    bit::VertexHelper::positionQuad(headQuad, r.x, r.y, z, spriteWidth, spriteHeight);
    bit::VertexHelper::colorQuad(headQuad, color);
    headSprite->applyToQuad(headQuad);
    if(!facingRight) bit::VertexHelper::flipQuad(headQuad, true, false);

    bit::VertexHelper::positionQuad(frontarmQuad, r.x, r.y, z, spriteWidth, spriteHeight);
    bit::VertexHelper::colorQuad(frontarmQuad, color);
    frontarmSprite->applyToQuad(frontarmQuad);
    if(!facingRight) bit::VertexHelper::flipQuad(frontarmQuad, true, false);

    bit::VertexHelper::positionQuad(bodyQuad, r.x, r.y, z, spriteWidth, spriteHeight);
    bit::VertexHelper::colorQuad(bodyQuad, color);
    bodySprite->applyToQuad(bodyQuad);
    if(!facingRight) bit::VertexHelper::flipQuad(bodyQuad, true, false);

    bit::VertexHelper::positionQuad(shadowQuad, r.x, r.y, z, spriteWidth, spriteHeight);
    bit::VertexHelper::colorQuad(shadowQuad, color);
    shadowSprite->applyToQuad(shadowQuad);
    if(!facingRight) bit::VertexHelper::flipQuad(shadowQuad, true, false);

    if(equipmentHeadSprite)
    {
        bit::VertexHelper::positionQuad(equipmentHeadQuad, r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(equipmentHeadQuad, color);
        equipmentHeadSprite->applyToQuad(equipmentHeadQuad);
        if(!facingRight) bit::VertexHelper::flipQuad(equipmentHeadQuad, true, false);
    }
}

void CharacterSprite::reset()
{
    renderX = renderY = 0;
    bit::VertexHelper::resetQuad(headQuad);
    bit::VertexHelper::resetQuad(frontarmQuad);
    bit::VertexHelper::resetQuad(bodyQuad);
    bit::VertexHelper::resetQuad(shadowQuad);
}


/////////////////////////////////////////////////////////////
//                   EQUIPMENT SPRITING                    //
/////////////////////////////////////////////////////////////

void CharacterSprite::syncronizeEquipment()
{
    // If the character has a head slot and its not the same as ours, change it
    if(character->has_equipmentSlot_head && character->equipmentSlot_head.schema.type != equipmentHeadItemType)
    {
        unsetEquipmentHeadSprite();
        setEquipmentHeadSprite(character->equipmentSlot_head.schema.type);
    }
    // If the character does not have a head slot but we are rendering on, remove it
    else if(!character->has_equipmentSlot_head && equipmentHeadSprite)
    {
        unsetEquipmentHeadSprite();
    }

}
void CharacterSprite::setEquipmentHeadSprite(Item::Type itemType)
{
    equipmentHeadItemType = itemType;
    equipmentHeadSprite = spriteLoader->getSprite(Item::getSpriteName(character->equipmentSlot_head.schema.type));
    equipmentHeadSprite->applyToQuad(equipmentHeadQuad);
}
void CharacterSprite::unsetEquipmentHeadSprite()
{
    equipmentHeadItemType = Item::Type::None;
    if(equipmentHeadSprite)
    {
        bit::VertexHelper::resetQuad(equipmentHeadQuad);
        equipmentHeadSprite = NULL;
    }
}


/////////////////////////////////////////////////////////////
//                     BODY SPRITING                       //
/////////////////////////////////////////////////////////////

void CharacterSprite::setBodySprites(std::string& head, std::string& frontarm, std::string& body, std::string& shadow)
{
    setHeadSprite(head);
    setFrontarmSprite(frontarm);
    setBodySprite(body);
    setShadowSprite(shadow);
}

void CharacterSprite::setHeadSprite(std::string& spriteName)
{
    headSprite = spriteLoader->getSprite(spriteName);
    headSprite->applyToQuad(headQuad);
}

void CharacterSprite::setFrontarmSprite(std::string& spriteName)
{
    frontarmSprite = spriteLoader->getSprite(spriteName);
    frontarmSprite->applyToQuad(frontarmQuad);
}

void CharacterSprite::setBodySprite(std::string& spriteName)
{
    bodySprite = spriteLoader->getSprite(spriteName);
    bodySprite->applyToQuad(bodyQuad);
}

void CharacterSprite::setShadowSprite(std::string& spriteName)
{
    shadowSprite = spriteLoader->getSprite(spriteName);
    shadowSprite->applyToQuad(shadowQuad);
}
