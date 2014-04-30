#include "Chest.hpp"
#include "../../bitengine/Game.hpp"
#include "SFML/System.hpp"
#include "../Level.hpp"
#include "../Tile.hpp"
#include "../Character.hpp"
#include "../Structure.hpp"
#include "../Interaction.hpp"
#include <functional>

Chest::Chest()
    : Structure(),  schema()
{
}

void Chest::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Structure::load(_level, _id, Structure::Type::Chest, _x, _y, _level->tileWidth, _level->tileHeight);

    blockFoV = false;
    schema.isLocked = true;
}

void Chest::update(sf::Time &gameTime)
{
    Structure::update(gameTime);
}

void Chest::unlockWithKey(::Character* interactor)
{
    schema.isLocked = false;
}

void Chest::lockWithKey(::Character* interactor)
{
    schema.isLocked = true;
}

void Chest::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    Structure::prepareSnapshot(packet, full);

    packet << schema;
}

void Chest::prepareInteractionTree(bit::ServerPacket &packet)
{
    // Pack tile id
    if(schema.isLocked)
    {
        packet << sf::Uint32(3);
        packet << sf::Uint32(Interaction::Type::UnlockableWithKey);
        packet << sf::Uint32(Interaction::Type::UnlockableWithLockpick);
        packet << sf::Uint32(Interaction::Type::UnlockableWithBash);
    }
    else
    {
        packet << sf::Uint32(2);
        packet << sf::Uint32(Interaction::Type::LockableWithKey);
        packet << sf::Uint32(Interaction::Type::LockableWithLockpick);
    }
}