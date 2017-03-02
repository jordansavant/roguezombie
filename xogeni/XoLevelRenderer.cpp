#include "XoLevelRenderer.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/Graphics.hpp"
#include "../ResourcePath.h"
#include "LevelGenerator/XoTileMap.hpp"
#include "LevelGenerator/XoTile.hpp"
#include "XoGeniStateStart.hpp"
#include "XoGeniGame.hpp"

XoGeni::XoLevelRenderer::XoLevelRenderer(XoGeniStateStart* stateStart)
    : stateStart(stateStart), tileMap(NULL)
{
    stateStart->xoGeniGame->spriteLoader->loadSprites(resourcePath() + "xogeni/spritesheet.csv");
    texture.loadFromFile(resourcePath() + "xogeni/spritesheet.png");
    vertexMap_tiles.load(&texture, sf::PrimitiveType::Quads);
    groundSprite = stateStart->xoGeniGame->spriteLoader->getSprite("tile_ground");
}

XoGeni::XoLevelRenderer::~XoLevelRenderer()
{
}

void XoGeni::XoLevelRenderer::load(XoTileMap* tileMap)
{
    // Unload quads
    for(unsigned int i=0; i < tileQuadIndexes.size(); i++)
    {
        unsigned int quadIndex = tileQuadIndexes[i];
        bit::VertexHelper::resetQuad(&vertexMap_tiles.vertexArray[quadIndex]);
    }

    // Load new quads
    this->tileMap = tileMap;
    
    for(unsigned int i=0; i < tileMap->tiles.size(); i++)
    {
        // Get/Create quad index
        unsigned int quadIndex;
        if(i < tileQuadIndexes.size())
        {
            quadIndex = tileQuadIndexes[i];
        }
        else
        {
            quadIndex = vertexMap_tiles.requestVertexIndex();
            tileQuadIndexes.push_back(quadIndex);
        }
    }

    if(tileQuadIndexes.size() > tileMap->tiles.size())
    {
        int uhoh = 1;
    }
}

void XoGeni::XoLevelRenderer::update(sf::Time &gameTime)
{
    unsigned int renderDistanceX = groundSprite->width + 1;
    unsigned int renderDistanceY = groundSprite->height + 1;
    unsigned int renderZ = .5;
    sf::Color color = sf::Color::White;

    for(unsigned int i=0; i < tileMap->tiles.size(); i++)
    {
        XoTile* tile = tileMap->tiles[i];
        unsigned int quadIndex = quadIndex = tileQuadIndexes[i];

        unsigned int x = 15 + tile->x  * renderDistanceX;
        unsigned int y = 15 +tile->y  * renderDistanceY;

        bit::VertexHelper::positionQuad(&vertexMap_tiles.vertexArray[quadIndex], x, y, renderZ, groundSprite->width, groundSprite->height);
        bit::VertexHelper::colorQuad(&vertexMap_tiles.vertexArray[quadIndex], color);
        groundSprite->applyToQuad(&vertexMap_tiles.vertexArray[quadIndex]);
    }
}

void XoGeni::XoLevelRenderer::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    bit::VideoGame::depthTestBegin();

    // apply the transform
    states.transform *= getTransform();

    // non-characters
    states.texture = vertexMap_tiles.texture;
    target.draw(vertexMap_tiles.vertexArray, states);

    bit::VideoGame::depthTestEnd();
}

sf::Vector2i XoGeni::XoLevelRenderer::getMapRenderSize()
{
    sf::Vector2i size;
    size.x = tileMap->width * (groundSprite->width + 1);
    size.y = tileMap->height * (groundSprite->height + 1);

    return size;
}