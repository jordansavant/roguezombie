#pragma once
#ifndef RZ_CHARACTERSPRITE_H
#define RZ_CHARACTERSPRITE_H

#include "CharacterClient.hpp"
#include "items/Item.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"

class LevelClient;

class CharacterSprite
{
public:

    CharacterSprite(unsigned int width, unsigned int height, unsigned int baseOffsetX, unsigned int basOffsetY);

    CharacterClient* character;
    float renderX, renderY;
    unsigned int width, height, baseOffsetX, baseOffsetY;
    bool facingRight;
    float lastRenderX, lastRenderY;
    bit::VertexMap* vertexMap;
    bit::SpriteLoader* spriteLoader;

    // Equipment spriting
    Item::Type equipmentHeadItemType;
    bit::Sprite* equipmentHeadSprite;
    unsigned int equipmentHeadQuadIndex;
    bit::Vertex3* equipmentHeadQuad;
    void setEquipmentHeadSprite(Item::Type itemType);
    void unsetEquipmentHeadSprite();

    // Body spriting
    void setBodySprites(std::string& head, std::string& frontarm, std::string& body, std::string& shadow);

    bit::Sprite* headSprite;
    unsigned int headQuadIndex;
    bit::Vertex3* headQuad;
    void setHeadSprite(std::string& spriteName);

    bit::Sprite* frontarmSprite;
    unsigned int frontarmQuadIndex;
    bit::Vertex3* frontarmQuad;
    void setFrontarmSprite(std::string& spriteName);

    bit::Sprite* bodySprite;
    unsigned int bodyQuadIndex;
    bit::Vertex3* bodyQuad;
    void setBodySprite(std::string& spriteName);

    bit::Sprite* shadowSprite;
    unsigned int shadowQuadIndex;
    bit::Vertex3* shadowQuad;
    void setShadowSprite(std::string& spriteName);

    void load(CharacterClient* character, bit::SpriteLoader* spriteLoader, bit::VertexMap &vertexMap);

    void update(sf::Time &gameTime);
    
    void reset();

    void syncronizeEquipment();


};

#endif