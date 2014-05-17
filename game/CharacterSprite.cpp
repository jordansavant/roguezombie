#include "CharacterSprite.hpp"
#include "CharacterClient.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Math.hpp"

CharacterSprite::CharacterSprite(unsigned int width, unsigned int height, unsigned int baseOffsetX, unsigned int baseOffsetY)
    : renderX(0), renderY(0), width(width), height(height), baseOffsetX(baseOffsetX), baseOffsetY(baseOffsetY), facingRight(true), lastRenderX(0), lastRenderY(0),
      vertexMap(NULL), spriteLoader(NULL), headSprite(NULL), frontarmSprite(NULL), bodySprite(NULL), shadowSprite(NULL)
{
    equipmentProfiles.resize(Character::EquipmentSlot::_count);
}

void CharacterSprite::load(CharacterClient* _character, bit::SpriteLoader* _spriteLoader, bit::VertexMap &_vertexMap)
{
    character = _character;
    spriteLoader = _spriteLoader;
    vertexMap = &_vertexMap;

    // Allocate quads in reverse order for back to front rendered graphics
    // this allows us to use the same Z and properly sort the character vertices
    deathQuadIndex = vertexMap->requestVertexIndex();
    deathQuad = &vertexMap->vertexArray[deathQuadIndex];

    shadowQuadIndex = vertexMap->requestVertexIndex();
    shadowQuad = &vertexMap->vertexArray[shadowQuadIndex];
    
    bodyQuadIndex = vertexMap->requestVertexIndex();
    bodyQuad = &vertexMap->vertexArray[bodyQuadIndex];
    
    headQuadIndex = vertexMap->requestVertexIndex();
    headQuad = &vertexMap->vertexArray[headQuadIndex];
    
    for(unsigned int i=0; i < equipmentProfiles.size(); i++)
    {
        equipmentProfiles[i].quadIndex = vertexMap->requestVertexIndex();
        equipmentProfiles[i].quad = &vertexMap->vertexArray[equipmentProfiles[i].quadIndex];
    }
    
    frontarmQuadIndex = vertexMap->requestVertexIndex();
    frontarmQuad = &vertexMap->vertexArray[frontarmQuadIndex];
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
    r.x -= spriteWidth / 2;// + xFootOffset / 2;
    r.y -= yFootOffset;
    float z = bit::Math::calculateDrawDepth(r.y + spriteHeight);
    sf::Color color((int)(255.0f * character->BodyClient::schema.illumination), (int)(255.0f * character->BodyClient::schema.illumination), (int)(255.0f * character->BodyClient::schema.illumination));

    if(!character->schema.isDead())
    {
        // BODY SPRITES
        bit::VertexHelper::positionQuad(headQuad, r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(headQuad, color);
        headSprite->applyToQuad(headQuad);
        bit::VertexHelper::flipQuad(headQuad, !facingRight, false);

        bit::VertexHelper::positionQuad(frontarmQuad, r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(frontarmQuad, color);
        frontarmSprite->applyToQuad(frontarmQuad);
        bit::VertexHelper::flipQuad(frontarmQuad, !facingRight, false);

        bit::VertexHelper::positionQuad(bodyQuad, r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(bodyQuad, color);
        bodySprite->applyToQuad(bodyQuad);
        bit::VertexHelper::flipQuad(bodyQuad, !facingRight, false);

        bit::VertexHelper::positionQuad(shadowQuad, r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(shadowQuad, color);
        shadowSprite->applyToQuad(shadowQuad);
        bit::VertexHelper::flipQuad(shadowQuad, !facingRight, false);

        // EQUIPMENT SPRITES
        for(unsigned int i=0; i < equipmentProfiles.size(); i++)
        {
            if(equipmentProfiles[i].sprite)
            {
                bit::VertexHelper::positionQuad(equipmentProfiles[i].quad, r.x, r.y, z, spriteWidth, spriteHeight);
                bit::VertexHelper::colorQuad(equipmentProfiles[i].quad, color);
                equipmentProfiles[i].sprite->applyToQuad(equipmentProfiles[i].quad);
                bit::VertexHelper::flipQuad(equipmentProfiles[i].quad, !facingRight, false);
            }
        }

        bit::VertexHelper::resetQuad(deathQuad);
    }
    else
    {
        bit::VertexHelper::resetQuad(headQuad);
        bit::VertexHelper::resetQuad(frontarmQuad);
        bit::VertexHelper::resetQuad(bodyQuad);
        bit::VertexHelper::resetQuad(shadowQuad);
        for(unsigned int i=0; i < equipmentProfiles.size(); i++)
        {
            if(equipmentProfiles[i].sprite)
            {
                bit::VertexHelper::resetQuad(equipmentProfiles[i].quad);
            }
        }

        bit::VertexHelper::positionQuad(deathQuad, r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(deathQuad, color);
        deathSprite->applyToQuad(deathQuad);
    }
}

void CharacterSprite::reset()
{
    renderX = renderY = 0;
    bit::VertexHelper::resetQuad(headQuad);
    bit::VertexHelper::resetQuad(frontarmQuad);
    bit::VertexHelper::resetQuad(bodyQuad);
    bit::VertexHelper::resetQuad(shadowQuad);
    bit::VertexHelper::resetQuad(deathQuad);

    for(unsigned int i=0; i < equipmentProfiles.size(); i++)
    {
        bit::VertexHelper::resetQuad(equipmentProfiles[i].quad);
    }
}


/////////////////////////////////////////////////////////////
//                   EQUIPMENT SPRITING                    //
/////////////////////////////////////////////////////////////

void CharacterSprite::syncronizeEquipment()
{
    for(unsigned int i=0; i < Character::EquipmentSlot::_count; i++)
    {
        Character::EquipmentSlot slot = static_cast<Character::EquipmentSlot>(i);
        EquipmentProfile* profile = &equipmentProfiles[slot];
        // If the character has a slot and its not the same as ours, change it
        if(character->hasEquipment[slot] && character->equipment[slot].schema.type != profile->type)
        {
            unsetEquipmentSprite(slot);
            setEquipmentSprite(slot, character->equipment[slot].schema.type);
        }
        // If the character does not have a slot but we are rendering on, remove it
        else if(!character->hasEquipment[slot] && profile->sprite)
        {
            unsetEquipmentSprite(slot);
        }
    }
}

void CharacterSprite::setEquipmentSprite(Character::EquipmentSlot slot, Item::Type type)
{
    equipmentProfiles[slot].type = type;
    equipmentProfiles[slot].sprite = spriteLoader->getSprite(Item::getSpriteName(type));
    equipmentProfiles[slot].sprite->applyToQuad(equipmentProfiles[slot].quad);
}
void CharacterSprite::unsetEquipmentSprite(Character::EquipmentSlot slot)
{
    equipmentProfiles[slot].type = Item::Type::None;
    if(equipmentProfiles[slot].sprite)
    {
        bit::VertexHelper::resetQuad(equipmentProfiles[slot].quad);
        equipmentProfiles[slot].sprite = NULL;
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

void CharacterSprite::setDeathSprite(std::string& spriteName)
{
    deathSprite = spriteLoader->getSprite(spriteName);
    deathSprite->applyToQuad(deathQuad);
}
