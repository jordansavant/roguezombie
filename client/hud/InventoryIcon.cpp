#include "InventoryIcon.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "Hud.hpp"

InventoryIcon::InventoryIcon()
    : hud(NULL), quadIndex(0), map(NULL), sprite(NULL), color(sf::Color::White)
{
}

InventoryIcon::InventoryIcon(Hud* hud)
    : hud(hud), quadIndex(0), map(&hud->interfaceVertexMap), sprite(NULL), color(sf::Color::White)
{
    quadIndex = map->requestVertexIndex();
}

void InventoryIcon::set(std::string &spriteName)
{
    sprite = hud->state->rogueZombieGame->spriteLoader->getSprite(spriteName);
    bit::VertexHelper::resetQuad(&map->vertexArray[quadIndex]);
}

void InventoryIcon::position(int x, int y, int z, int width, int height, float scale)
{
    sprite->applyToQuad(&map->vertexArray[quadIndex]);
    bit::VertexHelper::positionQuad(&map->vertexArray[quadIndex], x, y, z, width, height, scale);
    bit::VertexHelper::colorQuad(&map->vertexArray[quadIndex], color);
}

void InventoryIcon::reset()
{
    bit::VertexHelper::resetQuad(&map->vertexArray[quadIndex]);
}