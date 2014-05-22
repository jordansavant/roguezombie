#include "TileClient.hpp"
#include "Tile.hpp"
#include "LevelClient.hpp"
#include "BodyClient.hpp"
#include "StateGamePlay.hpp"
#include "RogueZombieGame.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Math.hpp"
#include <sstream>

TileClient::TileClient()
    : schema(), level(NULL), sprite(NULL), quadIndex(0), renderX(0), width(64), height(32), renderY(0), centerRenderX(0), centerRenderY(0) 
{
}

TileClient::~TileClient()
{
}

bool TileClient::isCardinallyAdjacent(BodyClient* body)
{
    int x1 = schema.x;
    int y1 = schema.y;
    int width1 = schema.width;
    int height1 = schema.height;
    int x2 = body->schema.x;
    int y2 = body->schema.y;
    int width2 = body->schema.width;
    int height2 = body->schema.height;

    int top1 = y1;
    int top2 = y2;
    int bottom1 = y1 + height1;
    int bottom2 = y2 + height2;
    int left1 = x1;
    int left2 = x2;
    int right1 = x1 + width1;
    int right2 = x2 + width2;
    
    int xDiffA = std::abs(left1 - right2); // distance between my leftmost X and their rightmost X
    int xDiffB = std::abs(right1 - left2); // distance between my rightmost X and their leftmost X
    int xMin = std::min(xDiffA, xDiffB);

    int yDiffA = std::abs(bottom1 - top2); // distance between my bottom and their top
    int yDiffB = std::abs(top1 - bottom2); // distance between my top and their bottom
    int yMin = std::min(yDiffA, yDiffB);

    bool notInYPlane = (bottom1 <= top2 || top1 >= bottom2);
    bool inYPlane = !notInYPlane;

    bool notInXPlane = (left1 >= right2 || right1 <= left2);
    bool inXPlane = !notInXPlane;

    return (inXPlane || inYPlane) && (xMin == 0 || yMin == 0);
}

void TileClient::clientLoad(LevelClient* _level)
{
    level = _level;

    // Game quad
    quadIndex = level->vertexMap_01.requestVertexIndex();
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite("Water");
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}

void TileClient::clientUpdate(sf::Time &gameTime)
{
    // Sprite
    sprite->applyToQuad(&level->vertexMap_01.vertexArray[quadIndex]);

    // Position
    bit::Vertex3* quad = &level->vertexMap_01.vertexArray[quadIndex];
    sf::Vector2f isoPosition = bit::VectorMath::normalToIsometric(schema.x, schema.y);
    renderX = isoPosition.x - schema.width;
    renderY = isoPosition.y;
    bit::VertexHelper::positionQuad(quad, renderX, renderY, 0, width, height);
    centerRenderX = renderX + width / 2;
    centerRenderY = renderY + height / 2;

    // Test Mouse translation
    float mx = std::floor((float)level->mousePositionInWorld.x / (float)schema.width);
    float my = std::floor((float)level->mousePositionInWorld.y / (float)schema.height);
    float tx = std::floor((float)schema.x / (float)schema.width);
    float ty = std::floor((float)schema.y / (float)schema.height);

    if(level->state->mode == StateGamePlay::Mode::Free && tx == mx && ty == my)
    {
        int s = bit::Math::clamp(255 * schema.illumination * 4, 0, 255);
        sf::Color move(s, s, s);
        sf::Color interact(255, 0, 0);
        bit::VertexHelper::colorQuad(quad, schema.bodyId > 0 ? interact : move);

        // Set me as being hovered over
        level->hoveredTile = this;
    }
    else
    {
        int r = schema.rshade * schema.illumination;
        int g = schema.gshade * schema.illumination;
        int b = schema.bshade * schema.illumination;
        sf::Color c(r, g, b);
        bit::VertexHelper::colorQuad(quad, c);

        // Unset if I was previously set
        if(level->hoveredTile == this)
        {
            level->hoveredTile = NULL;
        }
    }
}

void TileClient::reset()
{
    bit::VertexHelper::resetQuad(&level->vertexMap_01.vertexArray[quadIndex]);
}

void TileClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    packet >> schema;
}