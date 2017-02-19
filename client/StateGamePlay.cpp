#include "StateGamePlay.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/System.hpp"
#include "../ResourcePath.h"
#include "RogueZombieGame.hpp"
#include "../server/GameplayServer.hpp"
#include "../server/ServerEvent.hpp"
#include "../server/ClientRequest.hpp"
#include "LevelClient.hpp"
#include "TileClient.hpp"
#include "CharacterClient.hpp"
#include "../server/Command.hpp"
#include "hud/Hud.hpp"
#include "hud/InteractionMenu.hpp"
#include "hud/StatBubble.hpp"
#include "hud/DialogMenu.hpp"
#include "hud/Inventory.hpp"
#include "hud/LootMenu.hpp"
#include "items/ItemClient.hpp"
#include <sstream>

StateGamePlay::StateGamePlay(bit::StateStack &stack, RogueZombieGame* _game, bool isClient, bool isHost)
    : bit::ClientServerState(stack, _game, isClient, isHost), rogueZombieGame(_game), levelClient(NULL), mode(Mode::Init), endGameReason(EndGameReason::Quit), fps(), isTileSelectActive(false)
{
    levelClient = new LevelClient();
    std::string fpsFontPath(resourcePath() + "Agency.ttf");
    fps.load(fpsFontPath, 10, 10);
    fps.fpsText.setColor(sf::Color(0, 0, 0));//(sf::Color(50, 50, 50));
    createCamera(rogueZombieGame, 0, 0, 1, 1);
    cameras[0]->panSpeed = 3;

    hud = new Hud(this);

    // Game play mode logic
    modeEnter.resize(Mode::_count, NULL);
    modeExit.resize(Mode::_count, NULL);
    modeUpdate.resize(Mode::_count, NULL);
    
    modeEnter[Mode::Joining] = std::bind(&StateGamePlay::modeOnEnterJoining, this);
    modeExit[Mode::Joining] = std::bind(&StateGamePlay::modeOnExitJoining, this);
    modeUpdate[Mode::Joining] = std::bind(&StateGamePlay::modeOnUpdateJoining, this, std::placeholders::_1);

    modeEnter[Mode::Free] = std::bind(&StateGamePlay::modeOnEnterFree, this);
    modeExit[Mode::Free] = std::bind(&StateGamePlay::modeOnExitFree, this);
    modeUpdate[Mode::Free] = std::bind(&StateGamePlay::modeOnUpdateFree, this, std::placeholders::_1);
    
    modeEnter[Mode::Loot] = std::bind(&StateGamePlay::modeOnEnterLoot, this);
    modeExit[Mode::Loot] = std::bind(&StateGamePlay::modeOnExitLoot, this);
    modeUpdate[Mode::Loot] = std::bind(&StateGamePlay::modeOnUpdateLoot, this, std::placeholders::_1);
    
    modeEnter[Mode::Interact] = std::bind(&StateGamePlay::modeOnEnterInteract, this);
    modeExit[Mode::Interact] = std::bind(&StateGamePlay::modeOnExitInteract, this);
    modeUpdate[Mode::Interact] = std::bind(&StateGamePlay::modeOnUpdateInteract, this, std::placeholders::_1);
    
    modeEnter[Mode::Dialog] = std::bind(&StateGamePlay::modeOnEnterDialog, this);
    modeExit[Mode::Dialog] = std::bind(&StateGamePlay::modeOnExitDialog, this);
    modeUpdate[Mode::Dialog] = std::bind(&StateGamePlay::modeOnUpdateDialog, this, std::placeholders::_1);
    
    modeEnter[Mode::Inventory] = std::bind(&StateGamePlay::modeOnEnterInventory, this);
    modeExit[Mode::Inventory] = std::bind(&StateGamePlay::modeOnExitInventory, this);
    modeUpdate[Mode::Inventory] = std::bind(&StateGamePlay::modeOnUpdateInventory, this, std::placeholders::_1);
    
    modeEnter[Mode::Journal] = std::bind(&StateGamePlay::modeOnEnterJournal, this);
    modeExit[Mode::Journal] = std::bind(&StateGamePlay::modeOnExitJournal, this);
    modeUpdate[Mode::Journal] = std::bind(&StateGamePlay::modeOnUpdateJournal, this, std::placeholders::_1);
    
    changeMode(Mode::Joining);
}

StateGamePlay::~StateGamePlay()
{
    delete hud;
    delete levelClient;
}

void StateGamePlay::load()
{
    bit::ClientServerState::load();
    levelClient->load(this);
}

void StateGamePlay::switchLevels(unsigned int newId)
{
    delete levelClient;
    levelClient = new LevelClient();
    levelClient->levelId = newId;
    levelClient->load(this);
}

void StateGamePlay::changeMode(Mode _mode)
{
    if(_mode != mode)
    {
        displayMessage(std::string("Changed modes"));
        if(mode != Mode::Init)
        {
            modeExit[mode]();
        }
        mode = _mode;
        modeEnter[mode]();
    }
}




/// /////////////////////////////////////////////////////////////////////
///                          JOINING MODES                             //
/// /////////////////////////////////////////////////////////////////////

void StateGamePlay::modeOnEnterJoining()
{
    hud->hide();
}

void StateGamePlay::modeOnExitJoining()
{
    hud->show();
}

void StateGamePlay::modeOnUpdateJoining(sf::Time &gameTime)
{
    // Exit to free mode when world has been initalized
    if(this->isConfirmed)
    {
        changeMode(Mode::Free);
    }
}




/// /////////////////////////////////////////////////////////////////////
///                            FREE MODES                              //
/// /////////////////////////////////////////////////////////////////////

void StateGamePlay::modeOnEnterFree()
{
}

void StateGamePlay::modeOnExitFree()
{
}

void StateGamePlay::modeOnUpdateFree(sf::Time &gameTime)
{
    // Move camera around
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Up))
        cameras[0]->direction.y = -1;
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Down))
        cameras[0]->direction.y = 1;
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Left))
        cameras[0]->direction.x = -1;
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Right))
        cameras[0]->direction.x = 1;

    // Debug commands to remove later on
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::X))
    {
        Command cmd;
        cmd.type = Command::Type::PlayerDebug;
        issueCommand(cmd);
    }
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::W))
    {
        Command cmd;
        cmd.type = Command::Type::PlayerMoveUp;
        issueCommand(cmd);
    }
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::S))
    {
        Command cmd;
        cmd.type = Command::Type::PlayerMoveDown;
        issueCommand(cmd);
    }
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::A))
    {
        Command cmd;
        cmd.type = Command::Type::PlayerMoveLeft;
        issueCommand(cmd);
    }
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::D))
    {
        Command cmd;
        cmd.type = Command::Type::PlayerMoveRight;
        issueCommand(cmd);
    }



    // Global Input Mappings
    isTileSelectActive = rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left);


    switch(levelClient->levelState)
    {
        case Level::State::Free:
        {
            if(levelClient->selectMode == LevelClient::SelectMode::None)
            {
                // Explore Mode Commands
                if(rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left))
                {
                    // See if a tile is being hovered over
                    if(levelClient->hoveredTile)
                    {
                        TileClient* t = levelClient->hoveredTile;

                        // If the tile has a body and it is adjacent run interactions
                        if(t->schema.bodyId > 0)
                        {
                            if(t->isCardinallyAdjacent(levelClient->playerCharacter))
                                requestInteractionsForTile(t->schema.id);
                            else if(t->hasInteractableBody())
                                displayMessage(std::string("Too far away"));
                            // Else we ignore the command
                        }
                        // Else issue command to move to tile
                        else
                        {
                            Command cmd;
                            cmd.type = Command::Type::FreeCommand;
                            cmd.pack = [t] (sf::Packet &packet) {
                                packet << sf::Uint32(Command::Target::Tile);
                                packet << sf::Uint32(t->schema.id);
                                packet << sf::Uint32(Command::TargetTileCommand::Move);
                            };
                            issueCommand(cmd);
                        }
                    }
                }
            }
            break;
        }
        case Level::State::Combat:
        {
            // Combat Mode Commands
            if(rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left))
            {
                // Always deactivate the stat bubble if something is clicked
                hud->statBubble->deactivate();

                // See if a tile is being hovered over
                if(levelClient->hoveredTile)
                {
                    TileClient* t = levelClient->hoveredTile;

                    // If the tile has a body and it is adjacent run interactions
                    if(t->schema.bodyId > 0)
                    {
                        if(t->hasTargetableCharacter())
                        {
                            target.tileId = t->schema.id;
                            target.bodyId = t->schema.bodyId;
                            hud->statBubble->handleStats(t->schema.id);
                        }
                    }
                    // Else issue command to move to tile
                    else
                    {
                        Command cmd;
                        cmd.type = Command::Type::CombatCommand;
                        cmd.pack = [t] (sf::Packet &packet) {
                            packet << sf::Uint32(Command::Target::Tile);
                            packet << sf::Uint32(t->schema.id);
                            packet << sf::Uint32(Command::TargetTileCommand::Move);
                        };
                        displayMessage(std::string("Player moves"));
                        issueCommand(cmd);
                    }
                }
            }
            break;
        }
    }

    

    // Menu Hot Keys
    modeOnUpdateCommonListener(gameTime);

    // Exit
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        endGame(EndGameReason::Quit);
    }
}


void StateGamePlay::onEnterCombat()
{
    displayMessage(std::string("Combat mode engaged"));
    hud->onEnterCombat();
    target.tileId = 0;
    target.bodyId = 0;

    // If I entering combat and I am not in loot or inventory mode, show the actionbar
    if(mode != Mode::Loot && mode != Mode::Inventory)
    {
        hud->activateActionBar();
    }
}

void StateGamePlay::onLeaveCombat()
{
    displayMessage(std::string("Free mode resumed"));
    hud->onLeaveCombat();
    target.tileId = 0;
    target.bodyId = 0;

    // If I leaving combat and I am not in loot or inventory mode, hide the actionbar
    if(mode != Mode::Loot && mode != Mode::Inventory)
    {
        hud->deactivateActionBar();
    }
}



/// /////////////////////////////////////////////////////////////////////
///                            LOOT MODES                              //
/// /////////////////////////////////////////////////////////////////////

void StateGamePlay::modeOnEnterLoot()
{
    hud->activateInventory(false);
    hud->lootMenu->activate();

    // If I am opening the inventpry and I am not in combat, show the action bar
    if(levelClient->levelState == Level::State::Free)
        hud->activateActionBar();
}

void StateGamePlay::modeOnExitLoot()
{
    serverRequest(
        [](bit::ClientPacket& packet) {
            packet << sf::Uint32(ClientRequest::CloseItemTransaction);
        },
        [](bit::ServerPacket& response){
        }
    );
    hud->lootMenu->deactivate();
    hud->deactivateInventory();

    // If I am closing the loot mode and I am not in combat hide the action bar
    if(levelClient->levelState == Level::State::Free)
    {
        hud->deactivateActionBar();
    }
}

void StateGamePlay::modeOnUpdateLoot(sf::Time &gameTime)
{
    // Exit
    modeOnUpdateCommonListener(gameTime);
}



/// /////////////////////////////////////////////////////////////////////
///                          INTERACT MODES                            //
/// /////////////////////////////////////////////////////////////////////

void StateGamePlay::modeOnEnterInteract()
{
    hud->interactionMenu->activate();
}

void StateGamePlay::modeOnExitInteract()
{
    hud->interactionMenu->deactivate();
}

void StateGamePlay::modeOnUpdateInteract(sf::Time &gameTime)
{
    // Exit
    modeOnUpdateCommonListener(gameTime);
}



/// /////////////////////////////////////////////////////////////////////
///                           DIALOG MODES                             //
/// /////////////////////////////////////////////////////////////////////

void StateGamePlay::modeOnEnterDialog()
{
    hud->dialogMenu->activate();
}

void StateGamePlay::modeOnExitDialog()
{
    hud->dialogMenu->deactivate();
}

void StateGamePlay::modeOnUpdateDialog(sf::Time &gameTime)
{
    // Exit
    modeOnUpdateCommonListener(gameTime);
}



/// /////////////////////////////////////////////////////////////////////
///                         INVENTORY MODES                            //
/// /////////////////////////////////////////////////////////////////////

void StateGamePlay::modeOnEnterInventory()
{
    hud->activateInventory(false);

    // If I am opening the inventpry and I am not in combat, show the action bar
    if(levelClient->levelState == Level::State::Free)
        hud->activateActionBar();
}

void StateGamePlay::modeOnExitInventory()
{
    hud->deactivateInventory();

    // If I am closing the inventory and I am not in combat hide the action bar
    if(levelClient->levelState == Level::State::Free)
        hud->deactivateActionBar();
}

void StateGamePlay::modeOnUpdateInventory(sf::Time &gameTime)
{
    modeOnUpdateCommonListener(gameTime);
}


/// /////////////////////////////////////////////////////////////////////
///                          JOURNAL MODES                             //
/// /////////////////////////////////////////////////////////////////////

void StateGamePlay::modeOnEnterJournal()
{
    hud->activateJournal(false);
}

void StateGamePlay::modeOnExitJournal()
{
    hud->deactivateJournal();
}

void StateGamePlay::modeOnUpdateJournal(sf::Time &gameTime)
{
    modeOnUpdateCommonListener(gameTime);
}



/// /////////////////////////////////////////////////////////////////////
///                          COMMON MODES                              //
/// /////////////////////////////////////////////////////////////////////

void StateGamePlay::modeOnUpdateCommonListener(sf::Time &gameTime)
{
    // Inventory hot key
    if(mode == Mode::Inventory && (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::I) || rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape)))
    {
        changeMode(Mode::Free);
        return;
    }
    if(mode != Mode::Inventory && mode != Mode::Loot && (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::I)))
    {
        changeMode(Mode::Inventory);
        return;
    }

    // Journal hot key
    if(mode == Mode::Journal && (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::J) || rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape)))
    {
        changeMode(Mode::Free);
        return;
    }
    if(mode != Mode::Journal && (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::J)))
    {
        changeMode(Mode::Journal);
        return;
    }

    // Loot hot key
    if(mode == Mode::Loot && (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::I) || rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape)))
    {
        changeMode(Mode::Free);
        return;
    }

    // Interact hot key
    if(mode == Mode::Interact && rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        changeMode(Mode::Free);
        return;
    }

    // Dialog hot key
    if(mode == Mode::Dialog && rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        changeMode(Mode::Free);
        return;
    }
}

void StateGamePlay::requestItemCommand(Item::Schema &itemSchema)
{
    // Depending on the item command type prepare targetting
    switch(itemSchema.commandType)
    {
        case Item::CommandType::CommandTypeSelf:
        {
            // Send immediate command
            Command cmd;
            cmd.type = Command::Type::ItemCommand;
            cmd.pack = [itemSchema] (sf::Packet &packet) {
                packet << itemSchema;
            };
            issueCommand(cmd);

            break;
        }

        case Item::CommandType::CommandTypeCharacter:
        {
            StateGamePlay* state = this;
            // Enter character targetting mode and when a character is clicked run this
            levelClient->enterCharacterSelectMode(itemSchema.effectiveRangeInTiles, [state, itemSchema] (CharacterClient* character, TileClient* tileClient) {
                Item::Schema itemSchemaX = itemSchema;

                Command cmd;
                cmd.type = Command::Type::ItemCommand;
                cmd.pack = [itemSchemaX, tileClient] (sf::Packet &packet) {
                    packet << itemSchemaX;
                    packet << sf::Uint32(tileClient->schema.id);
                };
                state->issueCommand(cmd);
            });
            break;
        }

        case Item::CommandType::CommandTypeArea:
        {
            StateGamePlay* state = this;
            // Enter area targetting mode and when a tile is clicked run this
            levelClient->enterAreaSelectMode(itemSchema.effectiveRangeInTiles, itemSchema.effectiveRadiusInTiles, [state, itemSchema] (TileClient* tileClient) {
                Item::Schema itemSchemaX = itemSchema;

                Command cmd;
                cmd.type = Command::Type::ItemCommand;
                cmd.pack = [itemSchemaX, tileClient] (sf::Packet &packet) {
                    packet << itemSchemaX;
                    packet << sf::Uint32(tileClient->schema.id);
                };
                state->issueCommand(cmd);
            });
            break;
        }
    }
}

void StateGamePlay::issueCommand(Command cmd)
{
    commandQueue.push_back(cmd);
}

bool StateGamePlay::handleInput(sf::Time &gameTime)
{
    State::handleInput(gameTime);

    if(rogueZombieGame->isInFocus)
    {
        levelClient->captureInput(gameTime);

        // Update gameplay mode input
        modeUpdate[mode](gameTime);
    }

    return true;
}

bool StateGamePlay::update(sf::Time &gameTime)
{
    bit::ClientServerState::update(gameTime);

    fps.update(gameTime);

    hud->update(*rogueZombieGame->renderWindow, gameTime);

    levelClient->update(gameTime);

    // Camera
    if(levelClient->playerCharacter)
    {
        float toleranceX = 250 * rogueZombieGame->currentResolutionRatioX;
        float toleranceY = 150 * rogueZombieGame->currentResolutionRatioY;

        sf::Vector2f position(levelClient->playerCharacter->BodyClient::schema.x, levelClient->playerCharacter->BodyClient::schema.y);
        position = bit::VectorMath::normalToIsometric(position.x, position.y);
        cameras[0]->lock(position.x, position.y, toleranceX, toleranceY, 4.0, 100.0);
    }

    return true;
}

void StateGamePlay::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::ClientServerState::draw(window, gameTime);

    hud->draw(window, gameTime);

    fps.draw(window, gameTime);
}

void StateGamePlay::drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera)
{
    window.draw(*levelClient);
}

bit::Server* StateGamePlay::newServer()
{
    return new GameplayServer();
}

std::string StateGamePlay::getServerIpAddress()
{
    // TODO: Hardcoded
    return "192.168.0.100";
}

unsigned short StateGamePlay::getServerPort()
{
    return BIT_SERVER_PORT;
}

void StateGamePlay::displayMessage(std::string &message)
{
    hud->displayMessage(message);
}

void StateGamePlay::displayPlayerMessage(CharacterClient* character, std::string &message)
{
    if(character == levelClient->playerCharacter)
    {
        displayMessage(message);
    }
}

void StateGamePlay::requestInteractionsForTile(unsigned int tileId)
{
    StateGamePlay* s = this;
    serverRequest(
        [tileId] (bit::ClientPacket& packet) // prepare
        {
            packet << sf::Uint32(ClientRequest::GetAvailableInteractionsForBodyOnTile);
            packet << sf::Uint32(tileId);
        },
        [tileId, s] (bit::ServerPacket& packet) // onComplete
        {
            bit::Output::Debug("Client request interaction options received");
            bool hasInteractions;
            packet >> hasInteractions;
            if(hasInteractions)
            {
                s->hud->interactionMenu->handleInteractionTree(packet, tileId);
                s->changeMode(StateGamePlay::Mode::Interact);
            }
        }
    );
}

void StateGamePlay::handleInteractionResponse(unsigned int tileId, Interaction::Type interaction, bit::ServerPacket &packet)
{
    switch(interaction)
    {
        case Interaction::Type::UnlockWithKey:
        case Interaction::Type::UnlockWithLockpick:
        case Interaction::Type::UnlockWithBash:
        {
            bool unlocked;
            packet >> unlocked;
            displayMessage(std::string(unlocked ? "Unlocked" : "Unlock failed"));
            requestInteractionsForTile(tileId);
            break;
        }
        case Interaction::Type::LockWithKey:
        case Interaction::Type::LockWithLockpick:
        {
            bool locked;
            packet >> locked;
            displayMessage(std::string(locked ? "Locked" : "Lock failed"));
            requestInteractionsForTile(tileId);
            break;
        }
        case Interaction::Type::OpenInventory:
        {
            bool givenAccess;
            packet >> givenAccess;

            if(givenAccess)
            {
                changeMode(Mode::Loot);
                hud->lootMenu->handleInventorySnapshot(packet, tileId);
            }
            else
            {
                displayMessage(std::string("Inventory is occupied"));
            }

            break;
        }
        case Interaction::Type::Dialog:
        {
            displayMessage(std::string("Dialog initiated"));
            changeMode(Mode::Dialog);
            hud->dialogMenu->handleDialogPacket(packet, tileId);

            break;
        }
    }
}

void StateGamePlay::endGame(EndGameReason reason)
{
    endGameReason = reason;
    disconnect();
}


/**
 * Packet handling
 */

void StateGamePlay::handlePacket_Broadcast(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle broadcast");
}

void StateGamePlay::handlePacket_InitializeSelf(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle initialize self");
}

void StateGamePlay::handlePacket_InitializeWorld(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle initialize world");

    sf::Uint32 levelId;
    packet >> levelId;
    levelClient->levelId = levelId;
    levelClient->handleSnapshot(packet, true);
    displayMessage(std::string("World initialized"));
}

void StateGamePlay::handlePacket_PeerClientConnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle client connected");
}

void StateGamePlay::handlePacket_ClientDisonnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle client disconnected");
}

void StateGamePlay::handlePacket_ServerUpdate(bit::ServerPacket &packet)
{
    //bit::Output::Debug("Client handle server update");

    sf::Uint32 levelId;
    bool isFullSnapshot;
    packet >> levelId;
    packet >> isFullSnapshot;

    if(levelClient->levelId != levelId)
    {
        switchLevels(levelId);
    }

    if(isFullSnapshot) bit::Output::Debug("received full");

    levelClient->handleSnapshot(packet, isFullSnapshot);
}

void StateGamePlay::handlePacket_ServerEvent(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle server event");

    ServerEvent eventType;
    bit::NetworkHelper::unpackEnum<sf::Uint32, ServerEvent>(packet, eventType);
    bit::Output::Debug((unsigned int)eventType);

    if(levelClient->playerCharacter)
    {
        switch(eventType)
        {
            case ServerEvent::LeavingLevel:
                displayMessage(std::string("Leaving level"));
                break;
            case ServerEvent::ArrivedLevel:
                displayMessage(std::string("Arrived at level"));
                unsigned int levelId;
                packet >> levelId;
                switchLevels(levelId);
                levelClient->handleSnapshot(packet, true);
                break;
            case ServerEvent::CannotTransitionInCombat:
                displayMessage(std::string("Cannot leave level in combat"));
                break;

            case ServerEvent::SpectateBegin:
                displayMessage(std::string("Spectate mode started"));
                break;
            case ServerEvent::SpectateEnd:
                displayMessage(std::string("Spectate mode ended"));
                break;

            case ServerEvent::MissionCompleted:
                levelClient->playerCharacter->handleServerEventPacket_missionCompleted(packet);
                break;
                
            case ServerEvent::InventoryUpdate:
                levelClient->playerCharacter->handleServerEventPacket_inventoryUpdate(packet);
                hud->inventory->buildItemList();
                break;
            case ServerEvent::ItemAdded:
                levelClient->playerCharacter->handleServerEventPacket_itemAdded(packet);
                break;
            case ServerEvent::ItemUpdated:
                levelClient->playerCharacter->handleServerEventPacket_itemUpdated(packet);
                break;
            case ServerEvent::ItemRemoved:
                levelClient->playerCharacter->handleServerEventPacket_itemRemoved(packet);
                break;
            case ServerEvent::EquipmentAdded:
                levelClient->playerCharacter->handleServerEventPacket_equipmentAdded(packet);
                break;
            case ServerEvent::EquipmentUpdated:
                levelClient->playerCharacter->handleServerEventPacket_equipmentUpdated(packet);
                break;
            case ServerEvent::EquipmentRemoved:
                levelClient->playerCharacter->handleServerEventPacket_equipmentRemoved(packet);
                break;
            case ServerEvent::EquipmentSwapped:
                levelClient->playerCharacter->handleServerEventPacket_equipmentSwapped(packet);
                break;

            case ServerEvent::CombatTurnStart:
                displayMessage(std::string("Turn begins"));
                bit::Output::Debug("Received TURN BEGINS");

                // If the targetted body is not the same, reset it
                if(target.active())
                {
                    bool bad = true;
                    auto t = levelClient->tiles.find(target.tileId);
                    if(t != levelClient->tiles.end())
                    {
                        if(t->second->hasTargetableCharacter() && t->second->schema.bodyId == target.bodyId)
                        {
                            bad = false;
                        }
                    }
                    if(bad)
                    {
                        target.reset();
                    }
                }

                break;
            case ServerEvent::CombatTurnEnd:
                displayMessage(std::string("Turn complete"));
                // combatTargettedTileId = 0;
                break;
            case ServerEvent::CombatDecisionStart:
                levelClient->handleCombatDecisionStart(packet);
                break;
            case ServerEvent::CombatDecisionEnd:
                levelClient->handleCombatDecisionEnd(packet);
                break;

            case ServerEvent::CharacterAttacksPlayer:
            {
                Character::Type ct;
                int amount;
                bit::NetworkHelper::unpackEnum<sf::Uint32, Character::Type>(packet, ct);
                packet >> amount;
                std::stringstream ss;
                ss << Character::getTitle(ct) << " attacks player for " << amount;
                displayMessage(ss.str());
                break;
            }
            case ServerEvent::PlayerAttacksCharacter:
            {
                Character::Type ct;
                int amount;
                bit::NetworkHelper::unpackEnum<sf::Uint32, Character::Type>(packet, ct);
                packet >> amount;
                std::stringstream ss;
                ss << "Player attacks " << Character::getTitle(ct) << " for " << amount;
                displayMessage(ss.str());
                break;
            }
            case ServerEvent::CharacterMissesPlayer:
            {
                Character::Type ct;
                bit::NetworkHelper::unpackEnum<sf::Uint32, Character::Type>(packet, ct);
                std::stringstream ss;
                ss << Character::getTitle(ct) << " attacks player and misses";
                displayMessage(ss.str());
                break;
            }
            case ServerEvent::PlayerMissesCharacter:
            {
                Character::Type ct;
                bit::NetworkHelper::unpackEnum<sf::Uint32, Character::Type>(packet, ct);
                std::stringstream ss;
                ss << "Player attacks " << Character::getTitle(ct) << " and misses";
                displayMessage(ss.str());
                break;
            }

            
            case ServerEvent::GameVictory:
            {
                endGame(EndGameReason::Victory);
                break;
            }
            case ServerEvent::GameDefeat:
            {
                endGame(EndGameReason::Defeat);
                break;
            }
        }
    }
}

void StateGamePlay::handlePacket_DisconnectAcknowledge(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle client disconnected acknowledged");

    switch(endGameReason)
    {
        default:
        case EndGameReason::Quit:
            requestStackPop();
            break;
        case EndGameReason::Victory:
            // TODO: after disconnect travel to credits state
            rogueZombieGame->errorMessage = "Game ended in victory.";
            requestStateClearTo(RogueZombieGame::stateGameError);
            break;
        case EndGameReason::Defeat:
            // TODO: after disconnect travel to credits state
            rogueZombieGame->errorMessage = "Game ended in defeat.";
            requestStateClearTo(RogueZombieGame::stateGameError);
            break;
    }
}

void StateGamePlay::handlePacket_Shutdown(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle server shutdown");

    // Error
    rogueZombieGame->errorMessage = "Server has been shut down.";
    requestStateClearTo(RogueZombieGame::stateGameError);
}

void StateGamePlay::handle_DisconnectTimeout()
{
    bit::Output::Debug("Client handle client disconnected ack timeout");

    // Exit
    requestStackPop();
}

void StateGamePlay::handlePacket_Kick(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle client kick");

    unsigned int kickCode;
    packet >> kickCode;

    switch(static_cast<GameplayServer::KickReason>(kickCode))
    {
        case GameplayServer::KickReason::NoSpawn:
            rogueZombieGame->errorMessage = "No room for spawn.";
            break;
        case GameplayServer::KickReason::CheatDetected:
            rogueZombieGame->errorMessage = "Cheating detected.";
            break;
    }

    requestStateClearTo(RogueZombieGame::stateGameError);
}

void StateGamePlay::handle_ServerTimeout()
{
    rogueZombieGame->errorMessage = "Connection to server has timed out.";
    requestStateClearTo(RogueZombieGame::stateGameError);
}

/**
 * Packet sending
 */

void StateGamePlay::preparePacket_ClientInformation(bit::ClientPacket &packet)
{
    bit::Output::Debug("Client prepare client information");
}

void StateGamePlay::preparePacket_ClientUpdate(bit::ClientPacket &packet)
{
    //bit::Output::Debug("Client prepare client update");

    // Notify of command count
    sf::Uint32 commandCount;
    commandCount = commandQueue.size();
    packet << commandCount;

    for(unsigned int i=0; i < commandQueue.size(); i++)
    {
        // Push commands onto packet, oldest first
        packet << static_cast<sf::Uint32>(commandQueue[i].type);
        commandQueue[i].pack(packet);
    }

    // Clear commands
    commandQueue.clear();
}

void StateGamePlay::preparePacket_ClientDisconnect(bit::ClientPacket &packet)
{
    bit::Output::Debug("Client prepare client disconnect");
}

void StateGamePlay::Target::reset()
{
    tileId = 0;
    bodyId = 0;
}

bool StateGamePlay::Target::active()
{
    return tileId > 0;
}