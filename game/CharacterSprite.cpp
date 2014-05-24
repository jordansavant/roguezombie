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
    shadowQuadIndex = vertexMap->requestVertexIndex();
    bodyQuadIndex = vertexMap->requestVertexIndex();
    headQuadIndex = vertexMap->requestVertexIndex();
    healthQuadIndex = vertexMap->requestVertexIndex();
    antiHealthQuadIndex = vertexMap->requestVertexIndex();
    for(unsigned int i=0; i < equipmentProfiles.size(); i++)
        equipmentProfiles[i].quadIndex = vertexMap->requestVertexIndex();
    frontarmQuadIndex = vertexMap->requestVertexIndex();
    healthQuadIndex = vertexMap->requestVertexIndex();
    antiHealthQuadIndex = vertexMap->requestVertexIndex();
    whiteSprite = spriteLoader->getSprite("White");

    syncSprites();
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
        bit::VertexHelper::positionQuad(&vertexMap->vertexArray[headQuadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(&vertexMap->vertexArray[headQuadIndex], color);
        headSprite->applyToQuad(&vertexMap->vertexArray[headQuadIndex]);
        bit::VertexHelper::flipQuad(&vertexMap->vertexArray[headQuadIndex], !facingRight, false);

        bit::VertexHelper::positionQuad(&vertexMap->vertexArray[frontarmQuadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(&vertexMap->vertexArray[frontarmQuadIndex], color);
        frontarmSprite->applyToQuad(&vertexMap->vertexArray[frontarmQuadIndex]);
        bit::VertexHelper::flipQuad(&vertexMap->vertexArray[frontarmQuadIndex], !facingRight, false);

        bit::VertexHelper::positionQuad(&vertexMap->vertexArray[bodyQuadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(&vertexMap->vertexArray[bodyQuadIndex], color);
        bodySprite->applyToQuad(&vertexMap->vertexArray[bodyQuadIndex]);
        bit::VertexHelper::flipQuad(&vertexMap->vertexArray[bodyQuadIndex], !facingRight, false);

        bit::VertexHelper::positionQuad(&vertexMap->vertexArray[shadowQuadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(&vertexMap->vertexArray[shadowQuadIndex], color);
        shadowSprite->applyToQuad(&vertexMap->vertexArray[shadowQuadIndex]);
        bit::VertexHelper::flipQuad(&vertexMap->vertexArray[shadowQuadIndex], !facingRight, false);

        // EQUIPMENT SPRITES
        for(unsigned int i=0; i < equipmentProfiles.size(); i++)
        {
            if(equipmentProfiles[i].sprite)
            {
                bit::VertexHelper::positionQuad(&vertexMap->vertexArray[equipmentProfiles[i].quadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
                bit::VertexHelper::colorQuad(&vertexMap->vertexArray[equipmentProfiles[i].quadIndex], color);
                equipmentProfiles[i].sprite->applyToQuad(&vertexMap->vertexArray[equipmentProfiles[i].quadIndex]);
                bit::VertexHelper::flipQuad(&vertexMap->vertexArray[equipmentProfiles[i].quadIndex], !facingRight, false);
            }
        }

        bit::VertexHelper::resetQuad(&vertexMap->vertexArray[deathQuadIndex]);

        if(character->level->levelState == Level::State::Combat)
        {
            // HEALTHBAR
            float ratio = (float)character->schema.health / (float)character->schema.maxHealth;
            int hw = spriteWidth * .75 * ratio;
            int hh = 2;
            int hx = r.x + (spriteWidth * .125);
            int hy = r.y + spriteHeight - (spriteHeight - baseOffsetY) + 10;
            whiteSprite->applyToQuad(&vertexMap->vertexArray[healthQuadIndex]);
            bit::VertexHelper::positionQuad(&vertexMap->vertexArray[healthQuadIndex], hx, hy, z, hw, hh);
            bit::VertexHelper::colorQuad(&vertexMap->vertexArray[healthQuadIndex], sf::Color(144, 219, 54));
            int aw = spriteWidth * .75 - hw;
            int ah = 2;
            int ax = r.x + spriteWidth - aw - (spriteWidth * .125);
            int ay = hy;
            whiteSprite->applyToQuad(&vertexMap->vertexArray[antiHealthQuadIndex]);
            bit::VertexHelper::positionQuad(&vertexMap->vertexArray[antiHealthQuadIndex], ax, ay, z, aw, ah);
            bit::VertexHelper::colorQuad(&vertexMap->vertexArray[antiHealthQuadIndex], sf::Color(243, 11, 0));
        }
        else
        {
            bit::VertexHelper::resetQuad(&vertexMap->vertexArray[healthQuadIndex]);
            bit::VertexHelper::resetQuad(&vertexMap->vertexArray[antiHealthQuadIndex]);
        }
    }
    else
    {
        bit::VertexHelper::resetQuad(&vertexMap->vertexArray[headQuadIndex]);
        bit::VertexHelper::resetQuad(&vertexMap->vertexArray[frontarmQuadIndex]);
        bit::VertexHelper::resetQuad(&vertexMap->vertexArray[bodyQuadIndex]);
        bit::VertexHelper::resetQuad(&vertexMap->vertexArray[shadowQuadIndex]);
        bit::VertexHelper::resetQuad(&vertexMap->vertexArray[healthQuadIndex]);
        bit::VertexHelper::resetQuad(&vertexMap->vertexArray[antiHealthQuadIndex]);
        for(unsigned int i=0; i < equipmentProfiles.size(); i++)
        {
            if(equipmentProfiles[i].sprite)
            {
                bit::VertexHelper::resetQuad(&vertexMap->vertexArray[equipmentProfiles[i].quadIndex]);
            }
        }

        bit::VertexHelper::positionQuad(&vertexMap->vertexArray[deathQuadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(&vertexMap->vertexArray[deathQuadIndex], color);
        deathSprite->applyToQuad(&vertexMap->vertexArray[deathQuadIndex]);
    }
}

void CharacterSprite::reset()
{
    renderX = renderY = 0;
    bit::VertexHelper::resetQuad(&vertexMap->vertexArray[headQuadIndex]);
    bit::VertexHelper::resetQuad(&vertexMap->vertexArray[frontarmQuadIndex]);
    bit::VertexHelper::resetQuad(&vertexMap->vertexArray[bodyQuadIndex]);
    bit::VertexHelper::resetQuad(&vertexMap->vertexArray[shadowQuadIndex]);
    bit::VertexHelper::resetQuad(&vertexMap->vertexArray[deathQuadIndex]);
    bit::VertexHelper::resetQuad(&vertexMap->vertexArray[healthQuadIndex]);
    bit::VertexHelper::resetQuad(&vertexMap->vertexArray[antiHealthQuadIndex]);

    for(unsigned int i=0; i < equipmentProfiles.size(); i++)
    {
        bit::VertexHelper::resetQuad(&vertexMap->vertexArray[equipmentProfiles[i].quadIndex]);
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
    equipmentProfiles[slot].sprite->applyToQuad(&vertexMap->vertexArray[equipmentProfiles[slot].quadIndex]);
}
void CharacterSprite::unsetEquipmentSprite(Character::EquipmentSlot slot)
{
    equipmentProfiles[slot].type = Item::Type::None;
    if(equipmentProfiles[slot].sprite)
    {
        bit::VertexHelper::resetQuad(&vertexMap->vertexArray[equipmentProfiles[slot].quadIndex]);
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
    headSprite->applyToQuad(&vertexMap->vertexArray[headQuadIndex]);
}

void CharacterSprite::setFrontarmSprite(std::string& spriteName)
{
    frontarmSprite = spriteLoader->getSprite(spriteName);
    frontarmSprite->applyToQuad(&vertexMap->vertexArray[frontarmQuadIndex]);
}

void CharacterSprite::setBodySprite(std::string& spriteName)
{
    bodySprite = spriteLoader->getSprite(spriteName);
    bodySprite->applyToQuad(&vertexMap->vertexArray[bodyQuadIndex]);
}

void CharacterSprite::setShadowSprite(std::string& spriteName)
{
    shadowSprite = spriteLoader->getSprite(spriteName);
    shadowSprite->applyToQuad(&vertexMap->vertexArray[shadowQuadIndex]);
}

void CharacterSprite::setDeathSprite(std::string& spriteName)
{
    deathSprite = spriteLoader->getSprite(spriteName);
    deathSprite->applyToQuad(&vertexMap->vertexArray[deathQuadIndex]);
}


void CharacterSprite::syncSprites()
{
    std::string moniker = getSpriteMoniker(character->schema.type);
    setBodySprites(std::string(moniker + "_Head"), std::string(moniker + "_FrontArm"), std::string(moniker + "_Body"), std::string(moniker + "_Shadow"));
    setDeathSprite(std::string(moniker + "_Dead"));
}

std::string CharacterSprite::getSpriteMoniker(Character::Type t)
{
    switch(t)
    {
        default:
            return "Undefined";
        case Character::Type::Zombie:
            return "Zombie";
        case Character::Type::Hunter:
            return "Hunter";
        case Character::Type::Ogre:
            return "Ogre";
    }
}