#pragma once
#ifndef RZ_CHARACTERSPRITE_H
#define RZ_CHARACTERSPRITE_H

#include "../server/items/Item.hpp"
#include "../server/Character.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"

class LevelClient;
class CharacterClient;

class CharacterSprite
{
public:

    CharacterSprite(unsigned int width, unsigned int height, unsigned int baseOffsetX, unsigned int basOffsetY);

    CharacterClient* character;
    float renderX, renderY;
    float screenX, screenY;
    unsigned int width, height, baseOffsetX, baseOffsetY;
    bool facingRight;
    float lastRenderX, lastRenderY;
    bit::VertexMap* highlightMap;
    bit::VertexMap* normalMap;
    bit::SpriteLoader* spriteLoader;

    struct EquipmentProfile
    {
        EquipmentProfile()
            : type(Item::Type::None), sprite(NULL), quadIndex(0)
        {
        }

        Item::Type type;
        bit::Sprite* sprite;
        unsigned int quadIndex;
    };

    // Equipment spriting
    std::vector<EquipmentProfile> equipmentProfiles;
    void setEquipmentSprite(Character::EquipmentSlot slot, Item::Type type);
    void unsetEquipmentSprite(Character::EquipmentSlot slot);

    // Body spriting
    void setBodySprites(std::string& head, std::string& frontarm, std::string& body, std::string& shadow);

    bit::Sprite* headSprite;
    unsigned int headQuadIndex;
    void setHeadSprite(std::string& spriteName);

    bit::Sprite* frontarmSprite;
    unsigned int frontarmQuadIndex;
    void setFrontarmSprite(std::string& spriteName);

    bit::Sprite* bodySprite;
    unsigned int bodyQuadIndex;
    void setBodySprite(std::string& spriteName);

    bit::Sprite* shadowSprite;
    unsigned int shadowQuadIndex;
    void setShadowSprite(std::string& spriteName);
    
    bit::Sprite* deathSprite;
    unsigned int deathQuadIndex;
    void setDeathSprite(std::string& spriteName);

    // Stat spriting
    bit::Sprite* whiteSprite;
    unsigned int healthQuadIndex;
    unsigned int antiHealthQuadIndex;

    void load(CharacterClient* character, bit::SpriteLoader* spriteLoader, bit::VertexMap &highlightMap, bit::VertexMap &normalMap);

    void update(sf::Time &gameTime);
    
    void reset();

    void syncronizeEquipment();

    void syncSprites();

    static std::string getSpriteMoniker(Character::Type);

};

#endif