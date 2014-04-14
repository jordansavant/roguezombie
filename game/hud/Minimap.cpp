#include "Minimap.hpp"
#include "Hud.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"

Minimap::Minimap()
{
}

void Minimap::load(Hud* _hud)
{
    hud = _hud;
    sprite = hud->state->rogueZombieGame->spriteLoader->getSprite("minimap_mark");
    vertexMap.load(&hud->interfaceTexture, sf::PrimitiveType::Quads);
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
    //bit::VideoGame::depthTestBegin();

    //// apply the transform
    //states.transform *= getTransform();

    //// apply the tileset texture
    //states.texture = vertexMap.texture;

    //// draw the vertex arrays z-sorted
    //target.draw(vertexMap.vertexArray, states);

    //bit::VideoGame::depthTestEnd();
}