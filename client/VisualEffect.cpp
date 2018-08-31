#include "VisualEffect.hpp"
#include "LevelClient.hpp"
#include "StateGamePlay.hpp"
#include "RogueZombieGame.hpp"
#include "RZConfig.hpp"

VisualEffect::VisualEffect()
    : VisualEffect(NULL)
{
}


VisualEffect::VisualEffect(LevelClient* level)
    : type(Type::None), isComplete(false), worldX(0), worldY(0), level(level)
{
    quadIndex = level->vertexMap_charactersNormal.requestVertexIndex();
}

void ::VisualEffect::load(Type _type, float _worldX, float _worldY)
{
    type = _type;
    worldX = _worldX;
    worldY = _worldY;
    switch (type)
    {
        default:
        case Type::Poison:
            timer.setNewDuration(0.5f);
            break;
    }
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite(getSpriteMoniker(type));
    sprite->applyToQuad(&level->vertexMap_charactersNormal.vertexArray[quadIndex]);
}

void VisualEffect::reset()
{
    type = Type::None;
    worldX = 0;
    worldY = 0;
    isComplete = false;
    timer.setNewDuration(0);
    bit::VertexHelper::resetQuad(&level->vertexMap_charactersNormal.vertexArray[quadIndex]);
}

void VisualEffect::update(sf::Time &gameTime)
{
    // See if we are complete
    if (timer.update(gameTime))
    {
        isComplete = true;
    }
    else
    {
        // Lets do this
        switch (type)
        {
            case Type::Poison:
            {
                bit::Vertex3* quad = &level->vertexMap_charactersNormal.vertexArray[quadIndex];
                sf::Vector2f isoPosition = bit::VectorMath::normalToIsometric(worldX, worldY);
                float x = isoPosition.x - sprite->width / 2 ;
                float y = isoPosition.y - timer.getCompletionRatio() * 35;
                float z = RZConfig::getDrawDepthForGameplay(worldY + level->tileHeight);
                char alpha = 255 * (1 - timer.getCompletionRatio());
                bit::VertexHelper::positionQuad(quad, x, y, z, sprite->width, sprite->height);
                bit::VertexHelper::colorQuad(quad, sf::Color(255, 255, 255, alpha));

                // render the poison icon at this position
                break;
            }
        }
    }
}

std::string VisualEffect::getSpriteMoniker(Type type)
{
    switch (type)
    {
        default:
            return "ve_Undefined";
        case Type::Poison:
            return "ve_Poison";
    }
}