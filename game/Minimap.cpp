#include "Minimap.hpp"
#include "LevelClient.hpp"
#include "ClientGameplayState.hpp"
#include "RogueZombieGame.hpp"

Minimap::Minimap()
{
}

void Minimap::load(LevelClient* _level, sf::Texture &_texture)
{
    level = _level;
    texture = &_texture;
    vertexMap.load(texture, sf::PrimitiveType::Quads);
    sprite = level->state->rogueZombieGame->spriteLoader->getSprite("Tickmark");
}

void Minimap::addPoint(unsigned int tileId, int x, int y)
{
    if(points.find(tileId) == points.end())
    {
        unsigned int quadIndex = vertexMap.requestVertexIndex();
        bit::Vertex3* quad = &vertexMap.vertexArray[quadIndex];
        sprite->applyToQuad(quad);

        sf::Vector2f minimapIso = bit::VectorMath::normalToIsometric(x / 4, y / 4);
        //sf::Vector2f minimapIso(x / 4, y / 4);
        bit::VertexHelper::positionQuad(quad, minimapIso.x, minimapIso.y, 1, 16, 8);

        points.insert(std::pair<unsigned int, sf::Vector2i>(tileId, sf::Vector2i(x, y)));
    }
}

void Minimap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    bit::VideoGame::depthTestBegin();

    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = vertexMap.texture;

    // draw the vertex arrays z-sorted
    target.draw(vertexMap.vertexArray, states);

    bit::VideoGame::depthTestEnd();
}