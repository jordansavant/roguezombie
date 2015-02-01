#include "LevelClient.hpp"
#include "LevelClientRunner.hpp"
#include "TileClient.hpp"
#include "StateGamePlay.hpp"
#include "RogueZombieGame.hpp"
#include "CharacterClient.hpp"
#include "structures/WallClient.hpp"
#include "structures/DoorClient.hpp"
#include "structures/ChestClient.hpp"
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
    : state(NULL), tileCount(0), tileRows(0), tileColumns(0), tileWidth(0), tileHeight(0), levelState(Level::State::Free), tilePool(), characterPool(), doorPool(), chestPool(), hoveredTile(NULL), playerTile(NULL), playerCharacter(NULL), isPlayerDecisionMode(false), isPlayerSpecating(false),
      selectMode(SelectMode::None), onCharacterSelect(NULL), selectRange(1), selectRadius(1)
{
}

LevelClient::~LevelClient()
{
    for(unsigned int i=0; i < runners.size(); i++)
    {
        delete runners[i];
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

void LevelClient::clearMoveMarkers()
{
    for(unsigned int i=0; i < moveMarkers.size(); i++)
    {
        moveMarkers[i].hide();
    }
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
        unsigned int i=0;
        LevelClient* lc = this;
        bit::FloodFill::compute(playerTile->schema.x / tileWidth, playerTile->schema.y / tileHeight,
            [lc, &i] (int x, int y, int depth) {
                TileClient* tile = lc->getTileAtIndices(x, y);
                bit::Output::Debug(x);
                bit::Output::Debug(y);
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
                return !tile || depth > lc->playerCharacter->schema.speed || (tile->hasBody && tile->bodyClient != lc->playerCharacter);
            }
        );
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
        bool addMini = true;

        // unpack tile
        TileClient* t = unpackNetworkEntity<TileClient>(packet, full, tiles, tilePool);
        if(!tileMap[t->schema.id])
        {
            // add to tile map
            tileMap[t->schema.id] = t;
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
                StructureClient* s;

                unsigned int structureType;
                packet >> structureType;
                switch(static_cast<Structure::Type>(structureType))
                {
                    case Structure::Type::Wall:
                        s = unpackNetworkEntity<WallClient>(packet, full, walls, wallPool);
                        addMini = false;
                        break;
                    case Structure::Type::Door:
                        s = unpackNetworkEntity<DoorClient>(packet, full, doors, doorPool);
                        break;
                    case Structure::Type::Chest:
                        s = unpackNetworkEntity<ChestClient>(packet, full, chests, chestPool);
                        break;
                }

                // Tile occupant data
                t->hasBody = true;
                t->hasStructure = false;
                t->bodyClient = s;
                t->structureClient = s;

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
        if(addMini)
        {
            //state->hud->minimap.addPoint(t->schema.id, t->schema.x, t->schema.y);
        }
    }

    // Delete missing entities
    for(unsigned int i=0; i < runners.size(); i++)
    {
        runners[i]->diffNetwork();
    }
}