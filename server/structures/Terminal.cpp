#include "Terminal.hpp"
#include "../../bitengine/Game.hpp"
#include "SFML/System.hpp"
#include "../Level.hpp"
#include "../Tile.hpp"
#include "../Structure.hpp"
#include "../Interaction.hpp"
#include "../Character.hpp"
#include "../ServerEvent.hpp"
#include <functional>

Terminal::Terminal()
    : Structure(), schema()
{
}

void Terminal::load(Level* _level, unsigned int _id, float _x, float _y)
{
    Structure::load(_level, _id, Structure::Type::Terminal, _x, _y, _level->tileWidth, _level->tileHeight);

    blockFoV = false;
    Body::schema.obstructionRatio = .35;
}

void Terminal::update(sf::Time &gameTime)
{
    Structure::update(gameTime);
}