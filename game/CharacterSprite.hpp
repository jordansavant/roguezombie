#pragma once
#ifndef RZ_CHARACTERSPRITE_H
#define RZ_CHARACTERSPRITE_H

#include "CharacterClient.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"

class LevelClient;

class CharacterSprite
{
public:

    CharacterSprite();

    CharacterClient* character;
    float renderX, renderY;
    bit::VertexMap* vertexMap;
    bit::SpriteLoader* spriteLoader;

    bit::Sprite* headSprite;
    unsigned int headQuadIndex;
    bit::Vertex3* headQuad;

    bit::Sprite* frontarmSprite;
    unsigned int frontarmQuadIndex;
    bit::Vertex3* frontarmQuad;

    bit::Sprite* bodySprite;
    unsigned int bodyQuadIndex;
    bit::Vertex3* bodyQuad;

    bit::Sprite* shadowSprite;
    unsigned int shadowQuadIndex;
    bit::Vertex3* shadowQuad;

    void load(CharacterClient* character, bit::SpriteLoader* spriteLoader, bit::VertexMap &vertexMap);

    void update(sf::Time &gameTime);
    
    void reset();

    void setHeadSprite(std::string& spriteName);
    void setFrontarmSprite(std::string& spriteName);
    void setBodySprite(std::string& spriteName);
    void setShadowSprite(std::string& spriteName);

};

#endif