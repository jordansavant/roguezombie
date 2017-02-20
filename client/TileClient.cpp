#include "TileClient.hpp"
#include "../server/Tile.hpp"
#include "LevelClient.hpp"
#include "BodyClient.hpp"
#include "CharacterClient.hpp"
#include "StructureClient.hpp"
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
    hasBody(false), hasCharacter(false), hasStructure(false), bodyClient(NULL), characterClient(NULL), structureClient(NULL),
    metadata_shadowcastId(0), metadata_floodfillId(0)
{
}

TileClient::~TileClient()
{
}

bool TileClient::hasTargetableCharacter()
{
    // Has a character that is not dead nor is the player
    return (hasCharacter && !characterClient->schema.isDead() && characterClient != level->playerCharacter);
}

bool TileClient::hasInteractableBody()
{
    // Has a character taht is not the player
    bool interactWithCharacter = (hasCharacter && characterClient != level->playerCharacter);

    bool interactWithStructure = (hasStructure && structureClient->isOfInteractableType());

    return interactWithCharacter || interactWithStructure;
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

    // Mouse translation
    float mx = std::floor((float)level->mousePositionInWorld.x / (float)schema.width);
    float my = std::floor((float)level->mousePositionInWorld.y / (float)schema.height);
    float tx = std::floor((float)schema.x / (float)schema.width);
    float ty = std::floor((float)schema.y / (float)schema.height);

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

    // Default coloring to the lighting engine
    sf::Color moveColor = getMoveColor();
    sf::Color interactColor = getInteractColor();
    sf::Color c;
    int r = schema.rshade * schema.illumination;
    int g = schema.gshade * schema.illumination;
    int b = schema.bshade * schema.illumination;
    c = sf::Color(r, g, b);

    // Highlighting based on game mode
    switch(level->state->mode)
    {
        // Normal game play mode (not in menus etc, just in exploration or combat)
        case StateGamePlay::Mode::Free:

            switch(level->levelState)
            {
                // Not in combat, just exploring
                case Level::State::Free:

                    // If I am being hovered over, highlight me depending on if I am a body interactor or just an empty tile
                    if(level->hoveredTile == this)
                    {
                        // If I have something on me but its not a player character
                        if(schema.bodyId > 0)
                        {
                            if(level->playerCharacter != NULL && hasCharacter && characterClient == level->playerCharacter)
                                c = moveColor;
                            else
                                c = interactColor;
                        }
                        else
                        {
                            c = moveColor;
                        }
                    }

                    break;

                // In combat
                case Level::State::Combat:

                    // If its the players turn
                    if(level->isPlayerDecisionMode)
                    {
                        // If the player has an active target, see if its me and highlight me
                        if(level->state->target.tileId == schema.id)
                        {
                            c = RZConfig::tileTargetedColor;
                        }
                        // If am being hovered highlight white if not a body on top of it
                        else if(level->hoveredTile == this && schema.bodyId == 0)
                        {
                            c = moveColor;
                        }
                        // Color the tile to indicate I can target what is on here
                        else if(level->hoveredTile == this && hasTargetableCharacter())
                        {
                            c = interactColor;
                        }
                    }

                    break;
            }

            break;
    }
    
    // Targeting helpers and highlighting based on Targeting mode
    switch(level->selectMode)
    {
        case LevelClient::SelectMode::Character: // such as I am throwing a brick at someone
        {
            // Game is in character selection target mode
            if(level->hoveredTile == this && level->playerCharacter)
            {
                // If I am being hovered and I am within range of the primary character
                if(bit::VectorMath::distance(schema.x, schema.y, level->playerCharacter->BodyClient::schema.x, level->playerCharacter->BodyClient::schema.y) <= level->selectRange * schema.width)
                {
                    // Highlight myself if I am within range
                    c = getInRangeColor();

                    // Listen for selection input
                    if(level->state->isTileSelectActive && this->hasCharacter && this->characterClient)
                    {
                        // Run the character select operation
                        level->runSelectCharacter(characterClient, this);
                    }
                }
                else
                {
                    // Highlight err because I am out of range
                    c = getOutOfRangeColor();
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
                    c = getInRangeColor();

                    // Listen for selection
                    if(level->state->isTileSelectActive)
                    {
                        level->runSelectArea(this);
                    }
                }
                else
                {
                    // Highlight err out of range
                    c = getOutOfRangeColor();
                }
            }
            // If I am not hovered but it is targeting a neighbor
            else if(level->hoveredTile && bit::VectorMath::distance(schema.x, schema.y, level->hoveredTile->schema.x, level->hoveredTile->schema.y) <= level->selectRadius * schema.width)
            {
                // If the primary targeted tile is within range
                float distanceToTargetedTile = bit::VectorMath::distance(schema.x, schema.y, level->hoveredTile->schema.x, level->hoveredTile->schema.y);
                float rangeOfCenterTargetTile = bit::VectorMath::distance(level->hoveredTile->schema.x, level->hoveredTile->schema.y, level->playerCharacter->BodyClient::schema.x, level->playerCharacter->BodyClient::schema.y);
                if(rangeOfCenterTargetTile <= level->selectRange * schema.width)
                {
                    // Highlight auxilliary select color
                    c = getInRangeRadiusColor(distanceToTargetedTile, level->selectRadius);
                }
                // No highlight otherwise
            }
            break;
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

sf::Color TileClient::getMoveColor()
{
    int moveR = bit::Math::clamp(RZConfig::tileMoveColor.r * schema.illumination * 4, 0, 255);
    int moveG = bit::Math::clamp(RZConfig::tileMoveColor.g * schema.illumination * 4, 0, 255);
    int moveB = bit::Math::clamp(RZConfig::tileMoveColor.b * schema.illumination * 4, 0, 255);
    return sf::Color(moveR, moveG, moveB);
}

sf::Color TileClient::getInteractColor()
{
    int moveR = bit::Math::clamp(RZConfig::tileInteractColor.r * schema.illumination * 4, 0, 255);
    int moveG = bit::Math::clamp(RZConfig::tileInteractColor.g * schema.illumination * 4, 0, 255);
    int moveB = bit::Math::clamp(RZConfig::tileInteractColor.b * schema.illumination * 4, 0, 255);
    return sf::Color(moveR, moveG, moveB);
}

sf::Color TileClient::getOutOfRangeColor()
{
    int moveR = bit::Math::clamp(RZConfig::tileTargetOutOfRange.r * schema.illumination * 4, 0, 255);
    int moveG = bit::Math::clamp(RZConfig::tileTargetOutOfRange.g * schema.illumination * 4, 0, 255);
    int moveB = bit::Math::clamp(RZConfig::tileTargetOutOfRange.b * schema.illumination * 4, 0, 255);
    return sf::Color(moveR, moveG, moveB);
}

sf::Color TileClient::getInRangeColor()
{
    int moveR = bit::Math::clamp(RZConfig::tileTargetInRange.r * schema.illumination * 4, 0, 255);
    int moveG = bit::Math::clamp(RZConfig::tileTargetInRange.g * schema.illumination * 4, 0, 255);
    int moveB = bit::Math::clamp(RZConfig::tileTargetInRange.b * schema.illumination * 4, 0, 255);
    return sf::Color(moveR, moveG, moveB);
}

sf::Color TileClient::getInRangeRadiusColor(float distanceToCenter, unsigned int radius)
{
    int r = schema.rshade * schema.illumination;
    int g = schema.gshade * schema.illumination;
    int b = schema.bshade * schema.illumination;

    float illumination = 1 - (distanceToCenter / (float)(radius * schema.width));
    int moveR = bit::Math::clamp(RZConfig::tileTargetInRangeRadius.r * illumination * 2.5, r, 255);
    int moveG = bit::Math::clamp(RZConfig::tileTargetInRangeRadius.g * illumination * 2.5, g, 255);
    int moveB = bit::Math::clamp(RZConfig::tileTargetInRangeRadius.b * illumination * 2.5, b, 255);
    return sf::Color(moveR, moveG, moveB);
}