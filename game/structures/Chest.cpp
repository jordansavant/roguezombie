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

void Chest::handleInteraction(Interaction::Type interaction, Body* interactor)
{
    switch(interaction)
    {
        case Interaction::Type::UnlockWithKey:
        case Interaction::Type::UnlockWithLockpick:
        case Interaction::Type::UnlockWithBash:
        {
            schema.isLocked = false;
            break;
        }
        case Interaction::Type::LockWithKey:
        case Interaction::Type::LockWithLockpick:
        {
            schema.isLocked = true;
            break;
        };
    }
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
        packet << sf::Uint32(Interaction::Type::UnlockWithKey);
        packet << sf::Uint32(Interaction::Type::UnlockWithLockpick);
        packet << sf::Uint32(Interaction::Type::UnlockWithBash);
    }
    else
    {
        packet << sf::Uint32(2);
        packet << sf::Uint32(Interaction::Type::LockWithKey);
        packet << sf::Uint32(Interaction::Type::LockWithLockpick);
    }
}