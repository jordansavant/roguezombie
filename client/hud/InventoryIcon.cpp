#include "InventoryIcon.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "Hud.hpp"

InventoryIcon::InventoryIcon()
    : hud(NULL), quadIndex(0), map(NULL), sprite(NULL)
{
}

InventoryIcon::InventoryIcon(Hud* hud)
    : hud(hud), quadIndex(0), map(&hud->interfaceVertexMap), sprite(NULL)
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
}

void InventoryIcon::reset()
{
    bit::VertexHelper::resetQuad(&map->vertexArray[quadIndex]);
}