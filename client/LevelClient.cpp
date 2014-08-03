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
#include "SFML/Network.hpp"
#include "MoveMarker.hpp"
#include <map>

LevelClient::LevelClient()
    : state(NULL), levelState(Level::State::Free), tilePool(), characterPool(), doorPool(), chestPool(), hoveredTile(NULL), playerCharacter(NULL), isPlayerDecisionMode(false)
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
    texture_spritesheet_01_unsmooth.loadFromFile(resourcePath() + "spritesheet_01.png");
    texture_spritesheet_01_smooth.loadFromFile(resourcePath() + "spritesheet_01.png");
    texture_spritesheet_01_smooth.setSmooth(true);
    vertexMap_01.load(&texture_spritesheet_01_unsmooth, sf::PrimitiveType::Quads);
    vertexMapCharacters.load(&texture_spritesheet_01_unsmooth, sf::PrimitiveType::Quads);
    shader.loadFromFile(resourcePath() + "ShaderOutline.frag", sf::Shader::Fragment);

    // Load game runners
    runners.push_back(new LevelClientRunner<TileClient>(this, &tiles, &tilePool, 2000));
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
    bit::VideoGame::depthTestBegin();

    // apply the transform
    states.transform *= getTransform();

    // non-characters
    states.texture = vertexMap_01.texture;
    target.draw(vertexMap_01.vertexArray, states);
    
    // characters with shader
    states.texture = vertexMapCharacters.texture;
    if(state->rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::LAlt))
    {
        states.shader = &shader;
    }
    target.draw(vertexMapCharacters.vertexArray, states);

    bit::VideoGame::depthTestEnd();
}

void LevelClient::clearMoveMarkers()
{
    for(unsigned int i=0; i < moveMarkers.size(); i++)
    {
        moveMarkers[i].hide();
    }
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
    // unpack the move markers
    unsigned int size;
    packet >> size;
    for(unsigned int i=0; i < size; i++)
    {
        // unpack basic tile information
        unsigned int tileId;
        int x, y;
        packet >> tileId >> x >> y;

        // Render movement marker
        if(i > 0)
        {
            sf::Color w(255, 255, 255);
            moveMarkers[i].renderAt(x, y, w);
        }
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

    // Update / Create all entities
    unsigned int tileCount;
    packet >> tileCount;
    for(unsigned int i=0; i < tileCount; i++)
    {
        bool addMini = true;
        TileClient* t = unpackNetworkEntity<TileClient>(packet, full, tiles, tilePool);

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
                if(c->schema.clientId == state->clientId)
                {
                    playerCharacter = c;
                }

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

                t->hasBody = true;
                t->hasStructure = false;
                t->bodyClient = s;
                t->structureClient = s;

                break;
            }
            default:
            case Body::Type::None:
            {
                t->hasBody = false;
                t->hasCharacter = false;
                t->hasStructure = false;
                break;
            }
        }

        if(addMini)
        {
            state->hud->minimap.addPoint(t->schema.id, t->schema.x, t->schema.y);
        }
    }

    // Delete missing entities
    for(unsigned int i=0; i < runners.size(); i++)
    {
        runners[i]->diffNetwork();
    }
}