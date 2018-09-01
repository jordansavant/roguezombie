#include "LevelClient.hpp"
#include "LevelClientRunner.hpp"
#include "TileClient.hpp"
#include "StateGamePlay.hpp"
#include "RogueZombieGame.hpp"
#include "CharacterClient.hpp"
#include "VisualEffect.hpp"
#include "structures/WallClient.hpp"
#include "structures/DoorClient.hpp"
#include "structures/ChestClient.hpp"
#include "structures/FurnishingClient.hpp"
#include "hud/Hud.hpp"
#include "hud/Minimap.hpp"
#include "hud/TurnQueue.hpp"
#include "../bitengine/Math.hpp"
#include "../bitengine/Network.hpp"
#include "../ResourcePath.h"
#include "../bitengine/System.hpp"
#include "SFML/Network.hpp"
#include "MoveMarker.hpp"
#include <map>

LevelClient::LevelClient()
    : state(NULL), tileCount(0), tileRows(0), tileColumns(0), tileWidth(0), tileHeight(0), levelState(Level::State::Free), tilePool(), characterPool(), doorPool(), chestPool(), furnishingPool(), hoveredTile(NULL), playerTile(NULL), playerCharacter(NULL), isPlayerDecisionMode(false), isPlayerSpecating(false),
      selectMode(SelectMode::None), onCharacterSelect(NULL), selectRange(1), selectRadius(1), renderMoveMarkersOnNextSnapshot(false), visualEffectPool()
{
}

LevelClient::~LevelClient()
{
    for(unsigned int i=0; i < runners.size(); i++)
    {
        delete runners[i];
    }

    for (unsigned int i = 0; i < activeVisualEffects.size(); i++)
    {
        delete activeVisualEffects[i];
    }
}

void LevelClient::load(StateGamePlay* _state)
{
    state = _state;

    // Assets
    state->rogueZombieGame->spriteLoader->loadSprites(resourcePath() + "spritesheet_01.csv");
    texture_spritesheetCharacters.loadFromFile(resourcePath() + "spritesheet_01.png");
    vertexMap_charactersNormal.load(&texture_spritesheetCharacters, sf::PrimitiveType::Quads);
    vertexMap_charactersToggleIlluminated.load(&texture_spritesheetCharacters, sf::PrimitiveType::Quads);
    vertexMap_charactersConstIlluminated.load(&texture_spritesheetCharacters, sf::PrimitiveType::Quads);
    illuminateShader.loadFromFile(resourcePath() + "Illuminate.frag", sf::Shader::Fragment);
    outlineShader.loadFromFile(resourcePath() + "Outline.frag", sf::Shader::Fragment);

    // Load game runners
    runners.push_back(new LevelClientTileRunner<TileClient>(this, &tiles, &tilePool, 2000, &tileMap));
    runners.push_back(new LevelClientRunner<CharacterClient>(this, &characters, &characterPool, 10));
    runners.push_back(new LevelClientRunner<WallClient>(this, &walls, &wallPool, 200));
    runners.push_back(new LevelClientRunner<DoorClient>(this, &doors, &doorPool, 10));
    runners.push_back(new LevelClientRunner<ChestClient>(this, &chests, &chestPool, 10));
    runners.push_back(new LevelClientRunner<FurnishingClient>(this, &furnishings, &furnishingPool, 10));

    // Fill pools
    for(unsigned int i=0; i < runners.size(); i++)
    {
        runners[i]->buildPool();
    }

    // Load move markers
    moveMarkers.resize(100);
    for(unsigned int i=0; i < moveMarkers.size(); i++)
    {
        moveMarkers[i].load(this);
    }

    // prep effects pool
    visualEffectPool.factoryMethod = [this]() -> VisualEffect* {
        VisualEffect* ve = new VisualEffect(this);
        return ve;
    };
}

void LevelClient::captureInput(sf::Time &gameTime)
{
    // Update inputs
    mousePositionInScreen = sf::Mouse::getPosition(*state->rogueZombieGame->renderWindow);
    mousePositionInIsoWorld = state->rogueZombieGame->renderWindow->mapPixelToCoords(mousePositionInScreen);
    mousePositionInWorld = bit::VectorMath::isometricToNormal(mousePositionInIsoWorld.x, mousePositionInIsoWorld.y);
}

void LevelClient::update(sf::Time &gameTime)
{
    // Update entities
    for(unsigned int i=0; i < runners.size(); i++)
    {
        runners[i]->update(gameTime);
    }

    // Update visual effects
    for (auto it = activeVisualEffects.begin(); it != activeVisualEffects.end();)
    {
        (*it)->update(gameTime);

        // See if we need to remove the effect
        if ((*it)->isComplete)
        {
            bit::Output::Debug("REMOVE VISUAL EFFECT");
            visualEffectPool.recycle((*it));
            it = activeVisualEffects.erase(it);
        }
        else
        {
            ++it; // iterate list pointer
        }
    }

}

void LevelClient::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
    bool illuminateRequested = state->rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::LAlt);

    bit::VideoGame::depthTestBegin();

    // apply the transform
    states.transform *= getTransform();

    // non-characters
    states.texture = vertexMap_charactersNormal.texture;
    target.draw(vertexMap_charactersNormal.vertexArray, states);
    
    // characters with illumination
    states.texture = vertexMap_charactersToggleIlluminated.texture;
    if(illuminateRequested)
    {
        states.shader = NULL;
        states.shader = &illuminateShader;
    }
    target.draw(vertexMap_charactersToggleIlluminated.vertexArray, states);

    // characters with outlining
    states.texture = vertexMap_charactersConstIlluminated.texture;
    states.shader = &illuminateShader;
    target.draw(vertexMap_charactersConstIlluminated.vertexArray, states);

    bit::VideoGame::depthTestEnd();
}

TileClient* LevelClient::getTileAtIndices(int x, int y)
{
    unsigned int index = x + (tileColumns * y);

    if(index < tileMap.size())
        return tileMap[index];
    return NULL;
}

TileClient* LevelClient::getTileAtWorldPosition(float x, float y)
{
    // Takes into account width of tiles
    int ix = x / tileWidth;
    int iy = y / tileHeight;

    return getTileAtIndices(ix, iy);
}

CharacterClient* LevelClient::getCharacterById(unsigned int id)
{
    auto itr = characters.find(id);
    if (itr != characters.end())
    {
        CharacterClient* cc = itr->second;
        return cc;
    }
    return NULL;
}

void LevelClient::renderMoveMarkers()
{
    unsigned int i=0;
    LevelClient* lc = this;
    bit::FloodFill::compute(playerTile->schema.x / tileWidth, playerTile->schema.y / tileHeight,
        [lc, &i] (int x, int y, int depth) {
            TileClient* tile = lc->getTileAtIndices(x, y);
            if(tile && tile->metadata_floodfillId != bit::FloodFill::floodfillId)
            {
                tile->metadata_floodfillId = bit::FloodFill::floodfillId;

                sf::Color w(255, 255, 255);
                lc->moveMarkers[i].renderAt(x * lc->tileWidth, y * lc->tileHeight, w);
                i++;
            }
        },
        [lc] (int x, int y, int depth) -> bool {
            TileClient* tile = lc->getTileAtIndices(x, y);
            return !tile || depth > (lc->playerCharacter->schema.speed - lc->playerCharacter->schema.combatTilesTraversed) || (tile->hasBody && tile->bodyClient != lc->playerCharacter);
        }
    );
}

void LevelClient::clearMoveMarkers()
{
    for(unsigned int i=0; i < moveMarkers.size(); i++)
    {
        moveMarkers[i].hide();
    }

    renderMoveMarkersOnNextSnapshot = false; // dont delay render if we were planning to
}

void LevelClient::cancelSelectMode()
{
    onCharacterSelect = NULL;
    onAreaSelect = NULL;
    selectRadius = 0;
    selectRange = 0;
    selectMode = SelectMode::None;
}

void LevelClient::visualEffect(VisualEffect::Type type, float worldX, float worldY)
{
    bit::Output::Debug("ADD VISUAL EFFECT");
    VisualEffect* ve = visualEffectPool.fetch();
    ve->load(type, worldX, worldY);
    activeVisualEffects.push_back(ve);
}

void LevelClient::enterCharacterSelectMode(unsigned int range, std::function<void(CharacterClient* characterClient, TileClient* tileCilent)> onSelect)
{
    selectMode = SelectMode::Character;
    onCharacterSelect = onSelect;
    selectRange = range;
}

void LevelClient::enterAreaSelectMode(unsigned int range, unsigned int radius, std::function<void(TileClient* tileCilent)> onSelect)
{
    selectMode = SelectMode::Area;
    onAreaSelect = onSelect;
    selectRange = range;
    selectRadius = radius;
}

void LevelClient::runSelectCharacter(CharacterClient* characterClient, TileClient* tileClient)
{
    onCharacterSelect(characterClient, tileClient);
    cancelSelectMode();
}

void LevelClient::runSelectArea(TileClient* tileClient)
{
    onAreaSelect(tileClient);
    cancelSelectMode();
}

void LevelClient::onEnterCombat()
{
    state->onEnterCombat();
}

void LevelClient::onLeaveCombat()
{
    state->onLeaveCombat();
    clearMoveMarkers();
}

void LevelClient::handleCombatDecisionStart(bit::ServerPacket &packet)
{
    isPlayerDecisionMode = true;

    // build move markers via floodfill from player tile
    if(playerTile && playerCharacter)
    {
        // Delay rendering until we get our next update for the character
        // This is because the combat decision event can come before position data that caused it (such as them moving into combat)
        renderMoveMarkersOnNextSnapshot = true;
    }

    // unpack the chance of hit
    unsigned int characterSize;
    packet >> characterSize;
    for(unsigned int i=0; i < characterSize; i++)
    {
        float coh;
        unsigned int characterId;
        packet >> characterId >> coh;

        auto itr = characters.find(characterId);
        if(itr != characters.end())
        {
            CharacterClient* cc = itr->second;
            cc->chanceOfHit = coh;
        }
    }
}

void LevelClient::handleCombatDecisionEnd(bit::ServerPacket &packet)
{
    isPlayerDecisionMode = false;
    clearMoveMarkers();
}

void LevelClient::handleSnapshot(bit::ServerPacket &packet, bool full)
{
    // Game state
    packet >> tileCount;
    packet >> tileRows;
    packet >> tileColumns;
    packet >> tileWidth;
    packet >> tileHeight;
    if(full)
    {
        // For full world snapshots prep vectors
        tileMap.resize(tileCount, NULL);
    }

    Level::State lstate;
    bit::NetworkHelper::unpackEnum<sf::Uint32, Level::State>(packet, lstate);
    if(lstate != levelState)
    {
        if(lstate == Level::State::Free)
        {
            onLeaveCombat();
        }
        else
        {
            onEnterCombat();
        }
    }
    levelState = lstate;

    // State data
    switch(levelState)
    {
        case Level::State::Combat:
            // Send the turn queue of enums
            state->hud->turnQueue->handleTurnQueue(packet);
            break;
    }

    // Detect if we are spectating or playing
    bool isPlaying;
    packet >> isPlaying;
    isPlayerSpecating = !isPlaying;
    unsigned int playerBodyId;
    packet >> playerBodyId;

    // Update / Create all entities
    unsigned int packetTileCount;
    packet >> packetTileCount;
    for(unsigned int i=0; i < packetTileCount; i++)
    {
        Tile::Type minimarkerTileType = Tile::Type::None;
        Structure::Type minimarkerStructureType = Structure::Type::None;
        bool isMiniPlayer = false;

        // unpack tile
        TileClient* t = unpackNetworkEntity<TileClient>(packet, full, tiles, tilePool);
        if(!tileMap[t->schema.id])
        {
            // add to tile map
            tileMap[t->schema.id] = t;
            minimarkerTileType = t->schema.type;
        }

        // unpack body
        unsigned int bodyType;
        packet >> bodyType;
        switch(static_cast<Body::Type>(bodyType))
        {
            case Body::Type::Character:
            {
                CharacterClient* c;

                unsigned int characterType;
                packet >> characterType;
                c = unpackNetworkEntity<CharacterClient>(packet, full, characters, characterPool);

                // Player character
                if(c->BodyClient::schema.id == playerBodyId)
                {
                    playerCharacter = c;
                    playerTile = t;
                    isMiniPlayer = true;
                }

                // Tile occupant data
                t->hasBody = true;
                t->hasCharacter = true;
                t->bodyClient = c;
                t->characterClient = c;

                break;
            }
            case Body::Type::Structure:
            {
                StructureClient* s = NULL;

                unsigned int structureType;
                packet >> structureType;
                switch(static_cast<Structure::Type>(structureType))
                {
                    case Structure::Type::Wall:
                        s = unpackNetworkEntity<WallClient>(packet, full, walls, wallPool);
                        break;
                    case Structure::Type::Door:
                        s = unpackNetworkEntity<DoorClient>(packet, full, doors, doorPool);
                        break;
                    case Structure::Type::Chest:
                        s = unpackNetworkEntity<ChestClient>(packet, full, chests, chestPool);
                        break;
                    case Structure::Type::Furnishing:
                        s = unpackNetworkEntity<FurnishingClient>(packet, full, furnishings, furnishingPool);
                        break;
                }

                // Tile occupant data
                t->hasBody = true;
                t->hasStructure = true;
                t->bodyClient = s;
                t->structureClient = s;
                
                minimarkerStructureType = s->schema.type;

                break;
            }
            default:
            case Body::Type::None:
            {
                // Tile occupant data
                t->hasBody = false;
                t->hasCharacter = false;
                t->hasStructure = false;
                break;
            }
        }

        // Mini map tracker for viewed tiles
        state->hud->liveMinimap->addPoint(t->schema.id, t->schema.x, t->schema.y, minimarkerTileType, minimarkerStructureType);
        if(isMiniPlayer)
        {
            state->hud->liveMinimap->setPlayerPosition(t->schema.id, t->schema.x, t->schema.y);
        }
    }

    // Delete missing entities
    for(unsigned int i=0; i < runners.size(); i++)
    {
        runners[i]->diffNetwork();
    }

    // Render move markers if needed
    if(renderMoveMarkersOnNextSnapshot)
    {
        renderMoveMarkersOnNextSnapshot = false;
        renderMoveMarkers();
    }
}