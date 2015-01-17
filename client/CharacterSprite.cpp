#include "CharacterSprite.hpp"
#include "CharacterClient.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Math.hpp"
#include "RZConfig.hpp"

CharacterSprite::CharacterSprite(unsigned int width, unsigned int height, unsigned int baseOffsetX, unsigned int baseOffsetY)
    : character(NULL), cleanRender(true), renderX(0), renderY(0), width(width), height(height), baseOffsetX(baseOffsetX), baseOffsetY(baseOffsetY), facingRight(true), lastRenderX(0), lastRenderY(0),
      highlightMap(NULL), normalMap(NULL), spriteLoader(NULL), headSprite(NULL), frontarmSprite(NULL), bodySprite(NULL), shadowSprite(NULL)
{
    equipmentProfiles.resize(Character::EquipmentSlot::_count);
}

void CharacterSprite::load(CharacterClient* _character, bit::SpriteLoader* _spriteLoader, bit::VertexMap &_highlightMap, bit::VertexMap &_normalMap)
{
    character = _character;
    spriteLoader = _spriteLoader;
    highlightMap = &_highlightMap;
    normalMap = &_normalMap;

    // Allocate quads in reverse order for back to front rendered graphics
    // this allows us to use the same Z and properly sort the character vertices
    deathQuadIndex = highlightMap->requestVertexIndex();
    shadowQuadIndex = highlightMap->requestVertexIndex();
    bodyQuadIndex = highlightMap->requestVertexIndex();
    headQuadIndex = highlightMap->requestVertexIndex();
    for(unsigned int i=0; i < equipmentProfiles.size(); i++)
        equipmentProfiles[i].quadIndex = highlightMap->requestVertexIndex();
    frontarmQuadIndex = highlightMap->requestVertexIndex();

    healthQuadIndex = highlightMap->requestVertexIndex();
    antiHealthQuadIndex = highlightMap->requestVertexIndex();

    healthPositive = spriteLoader->getSprite("health_positive");
    healthNegative = spriteLoader->getSprite("health_negative");
    whiteSprite = spriteLoader->getSprite("White");

    syncSprites();
}

void CharacterSprite::update(sf::Time &gameTime)
{
    // Interpolate rendering position
    lastRenderX = renderX;
    lastRenderY = renderY;
    if(cleanRender || (renderX == 0 && renderY == 0))
    {
        renderX = character->BodyClient::schema.x;
        renderY = character->BodyClient::schema.y;
        lastRenderX = renderX;
        lastRenderY = renderY;
        cleanRender = false;
    }
    else
    {
        bit::VectorMath::incrementTowards(renderX, renderY, character->BodyClient::schema.x, character->BodyClient::schema.y, 4, 4);
    }
    facingRight = character->schema.direction.x > 0 || character->schema.direction.y < 0;

    // Calculate render position given sprite information and the isometric rendering
    float spriteWidth = (float)width;
    float spriteHeight = (float)height;
    float xFootOffset = (float)baseOffsetX;
    float yFootOffset = (float)baseOffsetY;
    float levelCenterX = renderX + character->BodyClient::schema.width / 2;
    float levelCenterY = renderY + character->BodyClient::schema.height / 2;
    sf::Vector2f r = bit::VectorMath::normalToIsometric(levelCenterX, levelCenterY);
    screenX = r.x;
    screenY = r.y;
    r.x -= spriteWidth / 2;// + xFootOffset / 2;
    r.y -= yFootOffset;
    float z = RZConfig::getDrawDepthForGameplay(r.y + spriteHeight);

    // Color and luminence
    int rr = character->BodyClient::schema.rshade * character->BodyClient::schema.illumination;
    int gg = character->BodyClient::schema.gshade * character->BodyClient::schema.illumination;
    int bb = character->BodyClient::schema.bshade * character->BodyClient::schema.illumination;
    sf::Color color(rr, gg, bb);

    // If someone is spectating this character illuminate
    if(character->level->isPlayerSpecating)
        color = sf::Color(0, 255, 255);

    if(!character->schema.isDead())
    {
        // BODY SPRITES
        bit::VertexHelper::positionQuad(&highlightMap->vertexArray[headQuadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(&highlightMap->vertexArray[headQuadIndex], color);
        headSprite->applyToQuad(&highlightMap->vertexArray[headQuadIndex]);
        bit::VertexHelper::flipQuad(&highlightMap->vertexArray[headQuadIndex], !facingRight, false);

        bit::VertexHelper::positionQuad(&highlightMap->vertexArray[frontarmQuadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(&highlightMap->vertexArray[frontarmQuadIndex], color);
        frontarmSprite->applyToQuad(&highlightMap->vertexArray[frontarmQuadIndex]);
        bit::VertexHelper::flipQuad(&highlightMap->vertexArray[frontarmQuadIndex], !facingRight, false);

        bit::VertexHelper::positionQuad(&highlightMap->vertexArray[bodyQuadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(&highlightMap->vertexArray[bodyQuadIndex], color);
        bodySprite->applyToQuad(&highlightMap->vertexArray[bodyQuadIndex]);
        bit::VertexHelper::flipQuad(&highlightMap->vertexArray[bodyQuadIndex], !facingRight, false);

        bit::VertexHelper::positionQuad(&highlightMap->vertexArray[shadowQuadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(&highlightMap->vertexArray[shadowQuadIndex], color);
        shadowSprite->applyToQuad(&highlightMap->vertexArray[shadowQuadIndex]);
        bit::VertexHelper::flipQuad(&highlightMap->vertexArray[shadowQuadIndex], !facingRight, false);

        // EQUIPMENT SPRITES
        for(unsigned int i=0; i < equipmentProfiles.size(); i++)
        {
            if(equipmentProfiles[i].sprite && i != Character::EquipmentSlot::WeaponSecondary)
            {
                bit::VertexHelper::positionQuad(&highlightMap->vertexArray[equipmentProfiles[i].quadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
                bit::VertexHelper::colorQuad(&highlightMap->vertexArray[equipmentProfiles[i].quadIndex], color);
                equipmentProfiles[i].sprite->applyToQuad(&highlightMap->vertexArray[equipmentProfiles[i].quadIndex]);
                bit::VertexHelper::flipQuad(&highlightMap->vertexArray[equipmentProfiles[i].quadIndex], !facingRight, false);
            }
        }

        bit::VertexHelper::resetQuad(&highlightMap->vertexArray[deathQuadIndex]);

        if(true || character->level->levelState == Level::State::Combat && character->schema.health < character->schema.maxHealth)
        {
            // HEALTHBAR
            float ratio = (float)character->schema.health / (float)character->schema.maxHealth;
            int hw = spriteWidth * .75 * ratio;
            int hh = 2;
            int hx = r.x + (spriteWidth * .125);
            int hy = r.y + spriteHeight - (spriteHeight - baseOffsetY) + 10;
            healthPositive->applyToQuad(&highlightMap->vertexArray[healthQuadIndex]);
            bit::VertexHelper::positionQuad(&highlightMap->vertexArray[healthQuadIndex], hx, hy, z, hw, hh);
            //bit::VertexHelper::colorQuad(&highlightMap->vertexArray[healthQuadIndex], sf::Color(144, 219, 54));
            int aw = spriteWidth * .75 - hw;
            int ah = 2;
            int ax = r.x + spriteWidth - aw - (spriteWidth * .125);
            int ay = hy;

            healthNegative->applyToQuad(&highlightMap->vertexArray[antiHealthQuadIndex]);
            bit::VertexHelper::positionQuad(&highlightMap->vertexArray[antiHealthQuadIndex], ax, ay, z, aw, ah);
            //bit::VertexHelper::colorQuad(&highlightMap->vertexArray[antiHealthQuadIndex], sf::Color(243, 11, 0));
        }
        else
        {
            bit::VertexHelper::resetQuad(&highlightMap->vertexArray[healthQuadIndex]);
            bit::VertexHelper::resetQuad(&highlightMap->vertexArray[antiHealthQuadIndex]);
        }
    }
    else
    {
        bit::VertexHelper::resetQuad(&highlightMap->vertexArray[headQuadIndex]);
        bit::VertexHelper::resetQuad(&highlightMap->vertexArray[frontarmQuadIndex]);
        bit::VertexHelper::resetQuad(&highlightMap->vertexArray[bodyQuadIndex]);
        bit::VertexHelper::resetQuad(&highlightMap->vertexArray[shadowQuadIndex]);
        bit::VertexHelper::resetQuad(&highlightMap->vertexArray[healthQuadIndex]);
        bit::VertexHelper::resetQuad(&highlightMap->vertexArray[antiHealthQuadIndex]);
        for(unsigned int i=0; i < equipmentProfiles.size(); i++)
        {
            if(equipmentProfiles[i].sprite)
            {
                bit::VertexHelper::resetQuad(&highlightMap->vertexArray[equipmentProfiles[i].quadIndex]);
            }
        }

        bit::VertexHelper::positionQuad(&highlightMap->vertexArray[deathQuadIndex], r.x, r.y, z, spriteWidth, spriteHeight);
        bit::VertexHelper::colorQuad(&highlightMap->vertexArray[deathQuadIndex], color);
        deathSprite->applyToQuad(&highlightMap->vertexArray[deathQuadIndex]);
    }
}

void CharacterSprite::reset()
{
    renderX = renderY = 0;
    bit::VertexHelper::resetQuad(&highlightMap->vertexArray[headQuadIndex]);
    bit::VertexHelper::resetQuad(&highlightMap->vertexArray[frontarmQuadIndex]);
    bit::VertexHelper::resetQuad(&highlightMap->vertexArray[bodyQuadIndex]);
    bit::VertexHelper::resetQuad(&highlightMap->vertexArray[shadowQuadIndex]);
    bit::VertexHelper::resetQuad(&highlightMap->vertexArray[deathQuadIndex]);
    bit::VertexHelper::resetQuad(&highlightMap->vertexArray[healthQuadIndex]);
    bit::VertexHelper::resetQuad(&highlightMap->vertexArray[antiHealthQuadIndex]);

    for(unsigned int i=0; i < equipmentProfiles.size(); i++)
    {
        bit::VertexHelper::resetQuad(&highlightMap->vertexArray[equipmentProfiles[i].quadIndex]);
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
    equipmentProfiles[slot].sprite->applyToQuad(&highlightMap->vertexArray[equipmentProfiles[slot].quadIndex]);
}
void CharacterSprite::unsetEquipmentSprite(Character::EquipmentSlot slot)
{
    equipmentProfiles[slot].type = Item::Type::None;
    if(equipmentProfiles[slot].sprite)
    {
        bit::VertexHelper::resetQuad(&highlightMap->vertexArray[equipmentProfiles[slot].quadIndex]);
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
    headSprite->applyToQuad(&highlightMap->vertexArray[headQuadIndex]);
}

void CharacterSprite::setFrontarmSprite(std::string& spriteName)
{
    frontarmSprite = spriteLoader->getSprite(spriteName);
    frontarmSprite->applyToQuad(&highlightMap->vertexArray[frontarmQuadIndex]);
}

void CharacterSprite::setBodySprite(std::string& spriteName)
{
    bodySprite = spriteLoader->getSprite(spriteName);
    bodySprite->applyToQuad(&highlightMap->vertexArray[bodyQuadIndex]);
}

void CharacterSprite::setShadowSprite(std::string& spriteName)
{
    shadowSprite = spriteLoader->getSprite(spriteName);
    shadowSprite->applyToQuad(&highlightMap->vertexArray[shadowQuadIndex]);
}

void CharacterSprite::setDeathSprite(std::string& spriteName)
{
    deathSprite = spriteLoader->getSprite(spriteName);
    deathSprite->applyToQuad(&highlightMap->vertexArray[deathQuadIndex]);
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