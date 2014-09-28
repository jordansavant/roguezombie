#include "TileClient.hpp"
#include "../server/Tile.hpp"
#include "LevelClient.hpp"
#include "BodyClient.hpp"
#include "CharacterClient.hpp"
#include "StateGamePlay.hpp"
#include "RogueZombieGame.hpp"
#include "RZConfig.hpp"
#include "SFML/Graphics.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Math.hpp"
#include <sstream>

TileClient::TileClient()
    : schema(), level(NULL), sprite(NULL), quadIndex(0), renderX(0), width(64), height(32), renderY(0), centerRenderX(0), centerRenderY(0),
    hasBody(false), hasCharacter(false), hasStructure(false), bodyClient(NULL), characterClient(NULL), structureClient(NULL)
{
}

TileClient::~TileClient()
{
}

bool TileClient::hasTargetableCharacter()
{
    return (hasCharacter && !characterClient->schema.isDead() && characterClient != level->playerCharacter);
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

    return bit::RectangleMath::isCardinallyAdjacent(x1, y1, width1, height1, x2, y2, width2, height2);
}

void TileClient::clientLoad(LevelClient* _level)
{
    level = _level;

    // Game quad
    quadIndex = level->vertexMap_charactersNormal.requestVertexIndex();
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite("Ground");
    sprite->applyToQuad(&level->vertexMap_charactersNormal.vertexArray[quadIndex]);
}

void TileClient::clientUpdate(sf::Time &gameTime)
{
    // Sprite
    sprite->applyToQuad(&level->vertexMap_charactersNormal.vertexArray[quadIndex]);

    // Position
    bit::Vertex3* quad = &level->vertexMap_charactersNormal.vertexArray[quadIndex];
    sf::Vector2f isoPosition = bit::VectorMath::normalToIsometric(schema.x, schema.y);
    renderX = isoPosition.x - schema.width;
    renderY = isoPosition.y;
    bit::VertexHelper::positionQuad(quad, renderX, renderY, RZConfig::zrenderTile, width, height);
    centerRenderX = renderX + width / 2;
    centerRenderY = renderY + height / 2;

    // Test Mouse translation
    float mx = std::floor((float)level->mousePositionInWorld.x / (float)schema.width);
    float my = std::floor((float)level->mousePositionInWorld.y / (float)schema.height);
    float tx = std::floor((float)schema.x / (float)schema.width);
    float ty = std::floor((float)schema.y / (float)schema.height);

    // Default coloring to the lighting engine
    sf::Color c;
    int r = schema.rshade * schema.illumination;
    int g = schema.gshade * schema.illumination;
    int b = schema.bshade * schema.illumination;
    c = sf::Color(r, g, b);

    // If I am being hovered
    if(tx == mx && ty == my)
    {
        // Set me as being hovered over
        level->hoveredTile = this;
    }
    // Unset if I was the previous tile hovered but not anymore
    else if(level->hoveredTile == this)
    {
        level->hoveredTile = NULL;
    }


    // Targetting helpers
    switch(level->selectMode)
    {
        case LevelClient::SelectMode::Character:
        {
            // Game is in character selection target mode
            if(level->hoveredTile == this && level->playerCharacter)
            {
                // If I am being hovered and I am within range of the primary character
                if(bit::VectorMath::distance(schema.x, schema.y, level->playerCharacter->BodyClient::schema.x, level->playerCharacter->BodyClient::schema.y) <= level->selectRange * schema.width)
                {
                    // Highlight
                    c = sf::Color(255, 255, 0);

                    // Listen for selection
                    if(level->state->isTileSelectActive && this->hasCharacter && this->characterClient)
                    {
                        level->onCharacterSelect(characterClient, this);
                        level->selectMode = LevelClient::SelectMode::None;
                    }
                }
                else
                {
                    // Highlight err
                    c = sf::Color(255, 0, 0);
                }
            }
            break;
        }

        case LevelClient::SelectMode::Area:
        {
            // Its targetting me
            if(level->hoveredTile == this && level->playerCharacter)
            {
                // If I am being hovered and I am within range of the primary character
                if(bit::VectorMath::distance(schema.x, schema.y, level->playerCharacter->BodyClient::schema.x, level->playerCharacter->BodyClient::schema.y) <= level->selectRange * schema.width)
                {
                    // Highlight
                    c = sf::Color(255, 255, 0);

                    // Listen for selection
                    if(level->state->isTileSelectActive)
                    {
                        level->onAreaSelect(this);
                        level->selectMode = LevelClient::SelectMode::None;
                    }
                }
                else
                {
                    // Highlight err
                    c = sf::Color(255, 0, 0);
                }
            }
            // Its targetting a neighbor
            else if(level->hoveredTile && bit::VectorMath::distance(schema.x, schema.y, level->hoveredTile->schema.x, level->hoveredTile->schema.y) <= level->selectRadius * schema.width)
            {
                // If the primary is within range
                if(bit::VectorMath::distance(level->hoveredTile->schema.x, level->hoveredTile->schema.y, level->playerCharacter->BodyClient::schema.x, level->playerCharacter->BodyClient::schema.y) <= level->selectRange * schema.width)
                {
                    // Highlight auxilliary select color
                    c = sf::Color(255, 255, 255);
                }
                // No highlight otherwise
            }
            break;
        }
    }


    if(false && level->state->mode == StateGamePlay::Mode::Free)
    {
        // If we are playing in free mode
        if(level->levelState == Level::State::Free)
        {
            // If I am being hovered over, highlight me depending on if I am a body interactor or just an empty tile
            if(level->hoveredTile == this)
            {
                int s = bit::Math::clamp(255 * schema.illumination * 4, 0, 255);
                sf::Color move(s, s, s);
                sf::Color interact(0, 255, 0);
                c = schema.bodyId > 0 ? interact : move;
            }
        }
        // If we are playing in combat mode
        else if(level->levelState == Level::State::Combat && level->isPlayerDecisionMode)
        {
            // If I am the currently targetted tile, highlight me
            if(level->state->target.tileId == schema.id)
            {
                c = sf::Color(0, 255, 0);
            }
            // If hovered highlight white if not a body (if moevable)
            else if(level->hoveredTile == this && schema.bodyId == 0)
            {
                int s = bit::Math::clamp(175 * schema.illumination * 4, 0, 175);
                c = sf::Color(s, s, s);
            }
            else if(level->hoveredTile == this && hasTargetableCharacter())
            {
                int s = bit::Math::clamp(175 * schema.illumination * 4, 0, 175);
                c = sf::Color(0, s, 0);
            }
        }
    }

    // Apply chosen coloring
    bit::VertexHelper::colorQuad(quad, c);
}

void TileClient::reinitialize()
{
}

void TileClient::reset()
{
    bit::VertexHelper::resetQuad(&level->vertexMap_charactersNormal.vertexArray[quadIndex]);
}

void TileClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    packet >> schema;
}