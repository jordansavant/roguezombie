#include "XoLevelRenderer.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/Graphics.hpp"
#include "../ResourcePath.h"
#include "LevelGenerator/CellMap.hpp"
#include "LevelGenerator/Cell.hpp"
#include "LevelGenerator/Room.hpp"
#include "XoGeniStateStart.hpp"
#include "XoGeniGame.hpp"

XoGeni::XoLevelRenderer::XoLevelRenderer(XoGeniStateStart* stateStart)
    : stateStart(stateStart), cellMap(NULL), renderState(0)
{
    stateStart->xoGeniGame->spriteLoader->loadSprites(resourcePath() + "xogeni/spritesheet.csv");
    texture.loadFromFile(resourcePath() + "xogeni/spritesheet.png");
    vertexMap_tiles.load(&texture, sf::PrimitiveType::Quads);
    groundSprite = stateStart->xoGeniGame->spriteLoader->getSprite("tile_ground");
}

XoGeni::XoLevelRenderer::~XoLevelRenderer()
{
}

void XoGeni::XoLevelRenderer::load(CellMap* cellMap)
{
    // Unload quads
    for(unsigned int i=0; i < tileQuadIndexes.size(); i++)
    {
        unsigned int quadIndex = tileQuadIndexes[i];
        bit::VertexHelper::resetQuad(&vertexMap_tiles.vertexArray[quadIndex]);
    }

    // Load new quads
    this->cellMap = cellMap;
    
    for(unsigned int i=0; i < cellMap->cells.size(); i++)
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

    if(tileQuadIndexes.size() > cellMap->cells.size())
    {
        int uhoh = 1;
    }

    paint();
}

void XoGeni::XoLevelRenderer::paint()
{
    /// RENDERING
    unsigned int renderDistanceX = groundSprite->width + 1;
    unsigned int renderDistanceY = groundSprite->height + 1;
    unsigned int renderZ = .5;

    for(unsigned int i=0; i < cellMap->cells.size(); i++)
    {
        Cell* cell = cellMap->cells[i];
        unsigned int quadIndex = quadIndex = tileQuadIndexes[i];

        unsigned int x = cell->x  * renderDistanceX;
        unsigned int y = cell->y  * renderDistanceY;
        
        sf::Color colorGround = sf::Color(30, 15, 0);
        sf::Color colorRoom = sf::Color(200, 200, 200);
        sf::Color colorRoomEdge = sf::Color(100, 100, 100);
        sf::Color colorRoomPerimeter = sf::Color(60, 30, 0);
        sf::Color colorTunnel = sf::Color(200, 200, 200);
        sf::Color colorSill = sf::Color::Cyan;
        sf::Color colorDoor = sf::Color::Yellow;

        sf::Color color = colorGround;

        unsigned int stateCount = 7;
        switch(renderState % stateCount)
        {
            case 1:
                if(cell->room)
                    color = colorRoom;
                break;
            case 2:
                if(cell->room)
                    if(cell->isRoomEdge)
                        color = colorRoomEdge;
                    else
                        color = colorRoom;
                if(cell->isRoomPermiter)
                    color = colorRoomPerimeter;
                break;
            case 3:
                if(cell->room)
                    if(cell->isRoomEdge)
                        color = colorRoomEdge;
                    else
                        color = colorRoom;
                if(cell->isRoomPermiter)
                    color = colorRoomPerimeter;
                if(cell->wasCorridorTunnel)
                    color = colorTunnel;
                break;
            case 4:
                if(cell->room)
                    if(cell->isRoomEdge)
                        color = colorRoomEdge;
                    else
                        color = colorRoom;
                if(cell->isRoomPermiter)
                    color = colorRoomPerimeter;
                if(cell->wasCorridorTunnel)
                    color = colorTunnel;
                if(cell->isSill)
                    color = colorSill;
                break;
            case 5:
                if(cell->room)
                    if(cell->isRoomEdge)
                        color = colorRoomEdge;
                    else
                        color = colorRoom;
                if(cell->isRoomPermiter)
                    color = colorRoomPerimeter;
                if(cell->wasCorridorTunnel)
                    color = colorTunnel;
                if(cell->isSill)
                    color = colorSill;
                if(cell->isDoor)
                    color = colorDoor;
                break;
            case 6:
                if(cell->room)
                    if(cell->isRoomEdge)
                        color = colorRoomEdge;
                    else
                        color = colorRoom;
                if(cell->isRoomPermiter)
                    color = colorRoomPerimeter;
                if(cell->wasCorridorTunnel)
                    color = colorTunnel;
                if(cell->wasDoorTunnel)
                    color = colorTunnel;
                if(cell->isSill)
                    color = colorSill;
                if(cell->isDoor)
                    color = colorDoor;
                break;
            // Full
            default:
            case 0:
                if(cell->room)
                    if(cell->isRoomEdge)
                        color = colorRoomEdge;
                    else
                        color = colorRoom;
                if(cell->isSill)
                    color = colorSill;
                if(cell->isRoomPermiter)
                    color = colorRoomPerimeter;
                if(cell->isTunnel)
                    color = colorTunnel;
                if(cell->isDoor)
                    color = sf::Color::Yellow;
                break;
        }
        

        bit::VertexHelper::positionQuad(&vertexMap_tiles.vertexArray[quadIndex], x, y, renderZ, groundSprite->width, groundSprite->height);
        bit::VertexHelper::colorQuad(&vertexMap_tiles.vertexArray[quadIndex], color);
        groundSprite->applyToQuad(&vertexMap_tiles.vertexArray[quadIndex]);
    }
}

void XoGeni::XoLevelRenderer::update(sf::Time &gameTime)
{
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
    size.x = cellMap->width * (groundSprite->width + 1);
    size.y = cellMap->height * (groundSprite->height + 1);

    return size;
}