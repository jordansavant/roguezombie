#pragma once
#ifndef RZ_WORLDCLIENT_H
#define RZ_WORLDCLIENT_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Structures.hpp"
#include <map>

class ZombieClient;
class OgreClient;
class TileClient;
class WallClient;
class GameplayState;

class WorldClient : public sf::Drawable, public sf::Transformable
{
public:

    WorldClient();

    ~WorldClient();

    GameplayState* state;
    std::map<unsigned int, ZombieClient*> zombies;
    std::map<unsigned int, OgreClient*> ogres;
    std::map<unsigned int, TileClient*> tiles;
    std::map<unsigned int, WallClient*> walls;
    bit::Pool<TileClient> tilePool;
    bit::Pool<ZombieClient> zombiePool;
    bit::Pool<OgreClient> ogrePool;
    bit::Pool<WallClient> wallPool;
    sf::Font font;
    TileClient* hoveredTile;

    // Spritesheet 01;
    sf::Texture texture_spritesheet_01;
    bit::VertexMap vertexMap_01;

    void load(GameplayState* state);

    void update(sf::RenderWindow &window, sf::Time &gameTime);

    void handleSnapshot(bit::ServerPacket &packet, bool full = false);

    void unpackTile(bit::ServerPacket &packet, bool full);

    void unpackBody(bit::ServerPacket &packet, bool full);

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif