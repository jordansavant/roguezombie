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
class TerminalClient;
class CharacterClient;

class LevelClient : public sf::Drawable, public sf::Transformable
{
public:

    LevelClient();

    virtual ~LevelClient();

    enum SelectMode
    {
        None,
        Character,
        Structure,
        Area,
    };

    unsigned int tileCount, tileRows, tileColumns;
    unsigned int tileWidth, tileHeight;
    StateGamePlay* state;
    unsigned int levelId;
    Level::State levelState;
    std::vector<BaseLevelClientRunner*> runners;
    std::vector<TileClient*> tileMap;
    std::map<unsigned int, CharacterClient*> characters;
    std::map<unsigned int, TileClient*> tiles;
    std::map<unsigned int, WallClient*> walls;
    std::map<unsigned int, DoorClient*> doors;
    std::map<unsigned int, ChestClient*> chests;
    std::map<unsigned int, TerminalClient*> terminals;
    bit::Pool<TileClient> tilePool;
    bit::Pool<CharacterClient> characterPool;
    bit::Pool<WallClient> wallPool;
    bit::Pool<DoorClient> doorPool;
    bit::Pool<ChestClient> chestPool;
    bit::Pool<TerminalClient> terminalPool;
    TileClient* hoveredTile;
    TileClient* playerTile;
    CharacterClient* playerCharacter;
    std::vector<MoveMarker> moveMarkers;
    bool renderMoveMarkersOnNextSnapshot;
    bool isPlayerDecisionMode;
    bool isPlayerSpecating;

    // Selection
    SelectMode selectMode;
    std::function<void(CharacterClient*, TileClient*)> onCharacterSelect;
    std::function<void(TileClient*)> onAreaSelect;
    unsigned int selectRange;
    unsigned int selectRadius;

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

    TileClient* getTileAtIndices(int x, int y);

    void renderMoveMarkers();

    void clearMoveMarkers();

    void cancelSelectMode();

    void enterCharacterSelectMode(unsigned int range, std::function<void(CharacterClient* characterClient, TileClient* tileCilent)>);

    void enterAreaSelectMode(unsigned int range, unsigned int radius, std::function<void(TileClient* tileCilent)>);

    void runSelectCharacter(CharacterClient* characterClient, TileClient* tileClient);

    void runSelectArea(TileClient* tileClient);

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
            // Update the located entity
            entity = itr->second;
            entity->level = this;
            entity->lastSnapshotId = state->lastSnapshotId; // update snapshot id
            entity->handleSnapshot(packet, full);
        }
        else
        {
            // Create the entity
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