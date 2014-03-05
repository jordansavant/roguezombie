#pragma once
#ifndef RZ_WORLDCLIENT_H
#define RZ_WORLDCLIENT_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include <map>

class ZombieClient;
class OgreClient;
class TileClient;
class WallClient;
class GameplayState;

class WorldClient
{
public:

    WorldClient();

    ~WorldClient();

    GameplayState* state;
    std::map<unsigned int, ZombieClient*> zombies;
    std::map<unsigned int, OgreClient*> ogres;
    std::map<unsigned int, TileClient*> tiles;
    std::map<unsigned int, WallClient*> walls;
    sf::Texture zombieimage;
    sf::Texture ogreimage;
    sf::Texture tileimage;
    sf::Texture wallimage;
    sf::Font font;

    void load(GameplayState* state);

    void update(sf::RenderWindow &window, sf::Time &gameTime);

    void draw(sf::RenderWindow &window, sf::Time &gameTime);

    void handleSnapshot(bit::ServerPacket &packet, bool full = false);
};

#endif