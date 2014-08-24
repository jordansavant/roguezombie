#pragma once
#ifndef RZ_WORLDCLIENT_H
#define RZ_WORLDCLIENT_H

#include "SFML/Graphics.hpp"
#include "SFML/Network.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/Structures.hpp"
#include "StateGamePlay.hpp"
#include "../server/Level.hpp"
#include "MoveMarker.hpp"
#include <map>

class BaseLevelClientRunner;
class ZombieClient;
class OgreClient;
class HunterClient;
class TileClient;
class WallClient;
class DoorClient;
class ChestClient;
class CharacterClient;

class LevelClient : public sf::Drawable, public sf::Transformable
{
public:

    LevelClient();

    virtual ~LevelClient();

    StateGamePlay* state;
    unsigned int levelId;
    Level::State levelState;
    std::vector<BaseLevelClientRunner*> runners;
    std::map<unsigned int, CharacterClient*> characters;
    std::map<unsigned int, TileClient*> tiles;
    std::map<unsigned int, WallClient*> walls;
    std::map<unsigned int, DoorClient*> doors;
    std::map<unsigned int, ChestClient*> chests;
    bit::Pool<TileClient> tilePool;
    bit::Pool<CharacterClient> characterPool;
    bit::Pool<WallClient> wallPool;
    bit::Pool<DoorClient> doorPool;
    bit::Pool<ChestClient> chestPool;
    TileClient* hoveredTile;
    CharacterClient* playerCharacter;
    std::vector<MoveMarker> moveMarkers;
    bool isPlayerDecisionMode;
    bool isPlayerSpecating;

    // Spritesheet 01;
    sf::Texture texture_spritesheetCharacters;
    bit::VertexMap vertexMap_charactersNormal;
    bit::VertexMap vertexMap_charactersToggleIlluminated;
    bit::VertexMap vertexMap_charactersConstIlluminated;
    sf::Shader outlineShader;
    sf::Shader illuminateShader;

    // Input
    sf::Vector2i mousePositionInScreen;
    sf::Vector2f mousePositionInIsoWorld;
    sf::Vector2f mousePositionInWorld;

    void load(StateGamePlay* state);

    void captureInput(sf::Time &gameTime);

    void update(sf::Time &gameTime);

    void clearMoveMarkers();

    void onEnterCombat();

    void onLeaveCombat();

    void handleCombatDecisionStart(bit::ServerPacket &packet);

    void handleCombatDecisionEnd(bit::ServerPacket &packet);

    void handleSnapshot(bit::ServerPacket &packet, bool full = false);

    template <class T>
    T* unpackNetworkEntity(bit::ServerPacket &packet, bool full, std::map<unsigned int, T*> &map, bit::Pool<T> &pool)
    {
        unsigned int id;
        packet >> id;

        T* entity;
        auto itr = map.find(id);
        if(itr != map.end())
        {
            entity = itr->second;
            entity->level = this;
            entity->lastSnapshotId = state->lastSnapshotId; // update snapshot id
            entity->handleSnapshot(packet, full);
        }
        else
        {
            entity = pool.fetch();
            map.insert(std::pair<unsigned int, T*>(id, entity));
            entity->level = this;
            entity->lastSnapshotId = state->lastSnapshotId; // update snapshot id
            entity->handleSnapshot(packet, full);
            entity->reinitialize();
        }

        return entity;
    }

private:

    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};

#endif