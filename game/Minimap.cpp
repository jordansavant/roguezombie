#include "Minimap.hpp"
#include "WorldClient.hpp"
#include "GameplayState.hpp"

Minimap::Minimap()
{
}

void Minimap::load(WorldClient* _world, sf::Texture &_texture)
{
    world = _world;
    texture = &_texture;
    vertexMap.load(texture, sf::PrimitiveType::Quads);
    sprite = world->state->game->spriteLoader->getSprite("Tickmark");
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
        bit::VertexHelper::positionQuad(quad, minimapIso.x - 450, minimapIso.y - 150, 1, 16, 8);

        points.insert(std::pair<unsigned int, sf::Vector2i>(tileId, sf::Vector2i(x, y)));
    }
}

void Minimap::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    bit::Game::depthTestBegin();

    // apply the transform
    states.transform *= getTransform();

    // apply the tileset texture
    states.texture = vertexMap.texture;

    // draw the vertex arrays z-sorted
    target.draw(vertexMap.vertexArray, states);

    bit::Game::depthTestEnd();
}