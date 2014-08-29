#include "InventoryIcon.hpp"
#include "../StateGamePlay.hpp"
#include "../RogueZombieGame.hpp"
#include "Hud.hpp"

InventoryIcon::InventoryIcon()
    : hud(NULL), quadIndex(0), map(NULL)
{
}

InventoryIcon::InventoryIcon(Hud* hud)
    : hud(hud), quadIndex(0), map(&hud->interfaceVertexMap)
{
    quadIndex = map->requestVertexIndex();
}

void InventoryIcon::set(std::string &spriteName)
{
    bit::Sprite* sprite = hud->state->rogueZombieGame->spriteLoader->getSprite(spriteName);
    sprite->applyToQuad(&map->vertexArray[quadIndex]);
}

void InventoryIcon::reset()
{
    bit::VertexHelper::resetQuad(&map->vertexArray[quadIndex]);
}