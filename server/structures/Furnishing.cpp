#include "Furnishing.hpp"
#include "../../bitengine/Game.hpp"
#include "SFML/System.hpp"
#include "../Level.hpp"
#include "../Tile.hpp"
#include "../Structure.hpp"
#include "../Interaction.hpp"
#include "../Character.hpp"
#include "../ServerEvent.hpp"
#include <functional>

Furnishing::Furnishing()
    : Structure(), schema()
{
    isDestroyable = true;
}

void Furnishing::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Structure::load(_level, _id, Structure::Type::Furnishing, _x, _y, _level->tileWidth, _level->tileHeight);

    blockFoV = false;
    Body::schema.obstructionRatio = .35;
}

void Furnishing::update(sf::Time &gameTime)
{
    Structure::update(gameTime);
}

void Furnishing::prepareSnapshot(bit::ServerPacket &packet, bool full)
{
    Structure::prepareSnapshot(packet, full);

    packet << schema;
}

std::string Furnishing::getTitle(SubType subtype)
{
    switch (subtype)
    {
        default:
            return "Undefined";
        case Furnishing::SubType::TerminalA:
        case Furnishing::SubType::RollingTerminalA:
        case Furnishing::SubType::TerminalB:
            return "Terminal";
        case Furnishing::SubType::ColumnA:
            return "Column";
        case Furnishing::SubType::DeskA:
        case Furnishing::SubType::DeskB:
        case Furnishing::SubType::DeskBeakerB:
        case Furnishing::SubType::DeskCoffeeA:
            return "Desk";
        case Furnishing::SubType::SinkA:
        case Furnishing::SubType::SinkB:
            return "Sink";
        case Furnishing::SubType::OpTableA:
            return "Op Table";
        case Furnishing::SubType::PilonA:
            return "Pilon";
        case Furnishing::SubType::TrashCanA:
        case Furnishing::SubType::TrashCanB:
            return "Trash Can";
        case Furnishing::SubType::ShelfA:
            return "Shelf";
        case Furnishing::SubType::HadesPortalA:
            return "Hades Portal";
        case Furnishing::SubType::HadesSpikeA:
        case Furnishing::SubType::HadesSpikeB:
            return "Hades Spike";
    }
}