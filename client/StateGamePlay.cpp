#include "StateGamePlay.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/System.hpp"
#include "../bitengine/Audio.hpp"
#include "../ResourcePath.h"
#include "RogueZombieGame.hpp"
#include "../server/GameplayServer.hpp"
#include "../server/ServerEvent.hpp"
#include "../server/ClientRequest.hpp"
#include "LevelClient.hpp"
#include "TileClient.hpp"
#include "CharacterClient.hpp"
#include "../server/Command.hpp"
#include "../server/CharacterEffect.hpp"
#include "menus/JoiningMenu.hpp"
#include "VisualEffect.hpp"
#include "hud/Hud.hpp"
#include "hud/InteractionMenu.hpp"
#include "hud/StatBubble.hpp"
#include "hud/DialogMenu.hpp"
#include "hud/Inventory.hpp"
#include "hud/LootMenu.hpp"
#include "hud/InformationPanel.hpp"
#include "items/ItemClient.hpp"
#include <sstream>

StateGamePlay::StateGamePlay(bit::StateStack &stack, RogueZombieGame* _game, bool isClient, bool isHost, bool isLocalOnly)
    : bit::ClientServerState(stack, _game, isClient, isHost, isLocalOnly), rogueZombieGame(_game), levelClient(NULL), mode(Mode::Init), endGameReason(EndGameReason::Quit), fps(),
    isHudInteractionOccurring(false), input_tileSelect(false), inputActive_tileSelect(false), lastInteractionDirectionCheckIndex(0)
{
    levelClient = new LevelClient();
    
    std::string fpsFontPath(resourcePath() + "Agency.ttf");
    fps.load(fpsFontPath, 5, 3);
    fps.fpsText.setCharacterSize(11);
    fps.fpsText.setFillColor(sf::Color(100, 100, 100));

    createCamera(rogueZombieGame, 0, 0, 1, 1);
    cameras[0]->panSpeed = 8;

    hud = new Hud(this);
    joiningMenu = new JoiningMenu(rogueZombieGame, this);

    // Sounds
    explosionSoundId = rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_explosion01.ogg");
    pistolGunshotSoundId = rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_pistol_01.ogg");
    rifleGunshotSoundId = rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_rifle_01.ogg");
    doorCloseSoundId = rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_click_01.ogg");
    openBodySoundId = rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_guts_01.ogg");
    closeBodySoundId = rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_guts_02.ogg");
    openChestSoundId = rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_chestopen_01.ogg");
    closeChestSoundId = rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_chestclose_01.ogg");
    humanCharacterDeathSoundId = rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_deathgrunt_01.ogg");
    thudSoundId = rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_thud_01.ogg");
    healSoundId = rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_heal_01.ogg");
    spikeReleaseSoundId = rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_spikerelease_01.ogg");
    spikeReturnSoundId = rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_spikereturn_01.ogg");
    whiffSoundId = rogueZombieGame->soundManager->loadSound(resourcePath() + "bit_wiff_01.ogg");

    // Music
    rogueZombieGame->musicManager->play(rogueZombieGame->exploreMusic);
    rogueZombieGame->musicManager->loop(rogueZombieGame->exploreMusic, true);

    // Game play mode logic
    modeEnter.resize(Mode::_count, NULL);
    modeExit.resize(Mode::_count, NULL);
    modeCaptureInput.resize(Mode::_count, NULL);
    modeUpdate.resize(Mode::_count, NULL);
    
    modeEnter[Mode::Joining] = std::bind(&StateGamePlay::modeOnEnterJoining, this);
    modeExit[Mode::Joining] = std::bind(&StateGamePlay::modeOnExitJoining, this);
    modeCaptureInput[Mode::Joining] = std::bind(&StateGamePlay::modeOnCaptureInputJoining, this, std::placeholders::_1);
    modeUpdate[Mode::Joining] = std::bind(&StateGamePlay::modeOnUpdateJoining, this, std::placeholders::_1);

    modeEnter[Mode::Free] = std::bind(&StateGamePlay::modeOnEnterFree, this);
    modeExit[Mode::Free] = std::bind(&StateGamePlay::modeOnExitFree, this);
    modeCaptureInput[Mode::Free] = std::bind(&StateGamePlay::modeOnCaptureInputFree, this, std::placeholders::_1);
    modeUpdate[Mode::Free] = std::bind(&StateGamePlay::modeOnUpdateFree, this, std::placeholders::_1);
    
    modeEnter[Mode::Loot] = std::bind(&StateGamePlay::modeOnEnterLoot, this);
    modeExit[Mode::Loot] = std::bind(&StateGamePlay::modeOnExitLoot, this);
    modeCaptureInput[Mode::Loot] = std::bind(&StateGamePlay::modeOnCaptureInputLoot, this, std::placeholders::_1);
    modeUpdate[Mode::Loot] = std::bind(&StateGamePlay::modeOnUpdateLoot, this, std::placeholders::_1);
    
    modeEnter[Mode::Interact] = std::bind(&StateGamePlay::modeOnEnterInteract, this);
    modeExit[Mode::Interact] = std::bind(&StateGamePlay::modeOnExitInteract, this);
    modeCaptureInput[Mode::Interact] = std::bind(&StateGamePlay::modeOnCaptureInputInteract, this, std::placeholders::_1);
    modeUpdate[Mode::Interact] = std::bind(&StateGamePlay::modeOnUpdateInteract, this, std::placeholders::_1);
    
    modeEnter[Mode::Dialog] = std::bind(&StateGamePlay::modeOnEnterDialog, this);
    modeExit[Mode::Dialog] = std::bind(&StateGamePlay::modeOnExitDialog, this);
    modeCaptureInput[Mode::Dialog] = std::bind(&StateGamePlay::modeOnCaptureInputDialog, this, std::placeholders::_1);
    modeUpdate[Mode::Dialog] = std::bind(&StateGamePlay::modeOnUpdateDialog, this, std::placeholders::_1);
    
    modeEnter[Mode::Inventory] = std::bind(&StateGamePlay::modeOnEnterInventory, this);
    modeExit[Mode::Inventory] = std::bind(&StateGamePlay::modeOnExitInventory, this);
    modeCaptureInput[Mode::Inventory] = std::bind(&StateGamePlay::modeOnCaptureInputInventory, this, std::placeholders::_1);
    modeUpdate[Mode::Inventory] = std::bind(&StateGamePlay::modeOnUpdateInventory, this, std::placeholders::_1);
    
    modeEnter[Mode::Journal] = std::bind(&StateGamePlay::modeOnEnterJournal, this);
    modeExit[Mode::Journal] = std::bind(&StateGamePlay::modeOnExitJournal, this);
    modeCaptureInput[Mode::Journal] = std::bind(&StateGamePlay::modeOnCaptureInputJournal, this, std::placeholders::_1);
    modeUpdate[Mode::Journal] = std::bind(&StateGamePlay::modeOnUpdateJournal, this, std::placeholders::_1);
    
    modeEnter[Mode::Options] = std::bind(&StateGamePlay::modeOnEnterOptions, this);
    modeExit[Mode::Options] = std::bind(&StateGamePlay::modeOnExitOptions, this);
    modeCaptureInput[Mode::Options] = std::bind(&StateGamePlay::modeOnCaptureInputOptions, this, std::placeholders::_1);
    modeUpdate[Mode::Options] = std::bind(&StateGamePlay::modeOnUpdateOptions, this, std::placeholders::_1);

	modeEnter[Mode::Dev] = std::bind(&StateGamePlay::modeOnEnterDev, this);
	modeExit[Mode::Dev] = std::bind(&StateGamePlay::modeOnExitDev, this);
	modeCaptureInput[Mode::Dev] = std::bind(&StateGamePlay::modeOnCaptureInputDev, this, std::placeholders::_1);
	modeUpdate[Mode::Dev] = std::bind(&StateGamePlay::modeOnUpdateDev, this, std::placeholders::_1);
    
    changeMode(Mode::Joining);

    // Interaction data
    interactionDirectionChecks.push_back(sf::Vector2f(-1, 0)); // left
    interactionDirectionChecks.push_back(sf::Vector2f(-1, -1)); // up left
    interactionDirectionChecks.push_back(sf::Vector2f(0, -1)); // up
    interactionDirectionChecks.push_back(sf::Vector2f(1, -1)); // up right
    interactionDirectionChecks.push_back(sf::Vector2f(1, 0)); // right
    interactionDirectionChecks.push_back(sf::Vector2f(1, 1)); // down right
    interactionDirectionChecks.push_back(sf::Vector2f(0, 1)); // down
    interactionDirectionChecks.push_back(sf::Vector2f(-1, 1)); // down left
}

StateGamePlay::~StateGamePlay()
{
    delete hud;
    delete joiningMenu;
    delete levelClient;
}

void StateGamePlay::load()
{
    bit::ClientServerState::load();
    levelClient->load(this);
}

void StateGamePlay::setGameZoom(bool setZoomed)
{
    if (setZoomed)
    {
        cameras[0]->changeZoom(100); // 2x
    }
    else
    {
        cameras[0]->resetZoom();
    }

    sf::Vector2f position(levelClient->playerCharacter->BodyClient::schema.x, levelClient->playerCharacter->BodyClient::schema.y);
    position = bit::VectorMath::normalToIsometric(position.x, position.y);
    cameras[0]->setCenter(position.x, position.y);
}

bool StateGamePlay::isGameZoomed()
{
    return cameras[0]->currentZoom > cameras[0]->baseZoom;
}

float StateGamePlay::getZoomRatio()
{
    return cameras[0]->getZoomRatio();
}

void StateGamePlay::switchLevels(unsigned int newId)
{
    hud->onLevelChange(levelClient->levelId, newId);

    delete levelClient;
    levelClient = new LevelClient();
    levelClient->levelId = newId;
    levelClient->load(this);
}

void StateGamePlay::onHoverTile(TileClient* tileClient)
{
    hud->informationPanel->display(tileClient);
}

void StateGamePlay::onUnhoverTile(TileClient* tileClient)
{
    hud->informationPanel->clear();
}

void StateGamePlay::changeMode(Mode _mode)
{
    if(_mode != mode)
    {
        if(mode != Mode::Init)
        {
            modeExit[mode]();
        }
        mode = _mode;
        modeEnter[mode]();
    }
}

void StateGamePlay::exitInventoryModes()
{
    switch(mode)
    {
        case Mode::Inventory:
        case Mode::Loot:
        case Mode::Journal:
        case Mode::Options:
            changeMode(Mode::Free);
            break;
    }
}




/// /////////////////////////////////////////////////////////////////////
///                          JOINING MODES                             //
/// /////////////////////////////////////////////////////////////////////

void StateGamePlay::modeOnEnterJoining()
{
    joiningMenu->show();
    hud->hide();
}

void StateGamePlay::modeOnExitJoining()
{
    joiningMenu->hide();
    hud->show();
}

void StateGamePlay::modeOnCaptureInputJoining(sf::Time &gameTime)
{
    // no input listening
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
    levelClient->cancelSelectMode();
}

void StateGamePlay::modeOnCaptureInputFree(sf::Time &gameTime)
{
    input_tileSelect = rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Left);

    modeOnCaptureInputCommonListener(gameTime);
}

void StateGamePlay::modeOnUpdateFree(sf::Time &gameTime)
{
    if (!rogueZombieGame->isInFocus)
        return;

    // Move camera around
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Up))
        cameras[0]->direction.y = -1;
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Down))
        cameras[0]->direction.y = 1;
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Left))
        cameras[0]->direction.x = -1;
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Right))
        cameras[0]->direction.x = 1;
    if (isShiftModifierDown && rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::RAlt))
        cameras[0]->resetZoom();
    else if (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::RAlt))
        cameras[0]->changeZoom(100);


    // Debug commands to remove later on
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::X))
    {
        Command cmd;
        cmd.type = Command::Type::PlayerDebug;
        issueCommand(cmd);
    }
    
    // Movement
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
    inputActive_tileSelect = input_tileSelect && !isHudInteractionOccurring;

    switch(levelClient->levelState)
    {
        case Level::State::Free:
        {
            if(levelClient->selectMode == LevelClient::SelectMode::None)
            {
                // Explore Mode Commands
                if(inputActive_tileSelect)
                {
                    // See if a tile is being hovered over
                    if(levelClient->hoveredTile)
                    {
                        TileClient* t = levelClient->hoveredTile;

                        // If the tile has a body and it is adjacent run interactions
                        if(t->schema.bodyId > 0)
                        {
                            if(t->isCardinallyOrDiagonallyAdjacent(levelClient->playerCharacter))
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
            if(levelClient->selectMode == LevelClient::SelectMode::None)
            {
                // Combat Mode Commands
                if(inputActive_tileSelect)
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
                            else if (t->hasInteractableBody())
                            {
                                if (t->isCardinallyOrDiagonallyAdjacent(levelClient->playerCharacter))
                                    requestInteractionsForTile(t->schema.id);
                                else
                                    displayMessage(std::string("Too far away"));
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
            }
            break;
        }
    }

    

    // Menu Hot Keys
    modeOnUpdateCommonListener(gameTime);

    // Escape operation
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        // If I am interacting with a selection then cancel it
        if(levelClient->selectMode != LevelClient::SelectMode::None)
        {
            levelClient->cancelSelectMode();
        }
        // Go to options menu to quit
        else if(mode != Mode::Options)
        {
            changeMode(Mode::Options);
        }
        // Return to game
        else
        {
            changeMode(Mode::Free);
        }
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

void StateGamePlay::modeOnCaptureInputLoot(sf::Time &gameTime)
{
    modeOnCaptureInputCommonListener(gameTime);
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

void StateGamePlay::modeOnCaptureInputInteract(sf::Time &gameTime)
{
    modeOnCaptureInputCommonListener(gameTime);
}

void StateGamePlay::modeOnUpdateInteract(sf::Time &gameTime)
{
    // Need to listen to numeric key events and pass them into the interaction menu so the user can select an option without clicking it

    if (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Num1))
        hud->interactionMenu->handleNumberInput(1);
    if (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Num2))
        hud->interactionMenu->handleNumberInput(2);
    if (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Num3))
        hud->interactionMenu->handleNumberInput(3);
    if (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Num4))
        hud->interactionMenu->handleNumberInput(4);
    if (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Num5))
        hud->interactionMenu->handleNumberInput(5);

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

void StateGamePlay::modeOnCaptureInputDialog(sf::Time &gameTime)
{
    modeOnCaptureInputCommonListener(gameTime);
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

void StateGamePlay::modeOnCaptureInputInventory(sf::Time &gameTime)
{
    modeOnCaptureInputCommonListener(gameTime);
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

void StateGamePlay::modeOnCaptureInputJournal(sf::Time &gameTime)
{
    modeOnCaptureInputCommonListener(gameTime);
}

void StateGamePlay::modeOnUpdateJournal(sf::Time &gameTime)
{
    modeOnUpdateCommonListener(gameTime);
}


/// /////////////////////////////////////////////////////////////////////
///                          OPTIONS MODES                             //
/// /////////////////////////////////////////////////////////////////////

void StateGamePlay::modeOnEnterOptions()
{
    hud->activateOptions(false);
}

void StateGamePlay::modeOnExitOptions()
{
    hud->deactivateOptions();
}

void StateGamePlay::modeOnCaptureInputOptions(sf::Time &gameTime)
{
    modeOnCaptureInputCommonListener(gameTime);
}

void StateGamePlay::modeOnUpdateOptions(sf::Time &gameTime)
{
    modeOnUpdateCommonListener(gameTime);
}


/// /////////////////////////////////////////////////////////////////////
///                            DEV MODES                               //
/// /////////////////////////////////////////////////////////////////////

void StateGamePlay::modeOnEnterDev()
{
	hud->activateDevTerminal();
}

void StateGamePlay::modeOnExitDev()
{
	hud->deactivateDevTerminal();
}

void StateGamePlay::modeOnCaptureInputDev(sf::Time &gameTime)
{
	// modeOnCaptureInputCommonListener(gameTime);
}

void StateGamePlay::modeOnUpdateDev(sf::Time &gameTime)
{
	// No common listener because I am capturing input
	// modeOnUpdateCommonListener(gameTime);
	if (mode == Mode::Dev && (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Tilde) || rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape)))
	{
		changeMode(Mode::Free);
		return;
	}
}


/// /////////////////////////////////////////////////////////////////////
///                          COMMON MODES                              //
/// /////////////////////////////////////////////////////////////////////

void StateGamePlay::modeOnCaptureInputCommonListener(sf::Time &gameTime)
{
}

void StateGamePlay::modeOnUpdateCommonListener(sf::Time &gameTime)
{
    if (!rogueZombieGame->isInFocus)
    {
        return;
    }

    // Interaction/Selection
    if (mode == Mode::Interact || mode == Mode::Free)
    {
        if (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::E))
        {
            TileClient* t = findNextInteractableTile();
            if (t)
            {
                requestInteractionsForTile(t->schema.id);
            }
        }
    }

    // Inventory hot key
    if (mode == Mode::Inventory && (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::I) || rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape)))
    {
        changeMode(Mode::Free);
        return;
    }
    if (mode != Mode::Inventory && mode != Mode::Loot && (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::I)))
    {
        changeMode(Mode::Inventory);
        return;
    }

    // Journal hot key
    if (mode == Mode::Journal && (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::J) || rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape)))
    {
        changeMode(Mode::Free);
        return;
    }
    if (mode != Mode::Journal && (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::J)))
    {
        changeMode(Mode::Journal);
        return;
    }

    // Options hot key
    if (mode == Mode::Options && (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::O) || rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape)))
    {
        changeMode(Mode::Free);
        return;
    }
    if (mode != Mode::Options && (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::O)))
    {
        changeMode(Mode::Options);
        return;
    }

    // Loot hot key
    if (mode == Mode::Loot && (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::I) || rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape)))
    {
        changeMode(Mode::Free);
        return;
    }

    // Interact hot key
    if (mode == Mode::Interact && rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        changeMode(Mode::Free);
        return;
    }

    // Dialog hot key
    if (mode == Mode::Dialog && rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        changeMode(Mode::Free);
        return;
    }

    // Dev hot key
    if (mode != Mode::Dev && (rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Tilde)))
    {
        changeMode(Mode::Dev);
        return;
    }
}

void StateGamePlay::requestItemCommand(Item::Schema &itemSchema, std::function<void()> onComplete)
{
    // Depending on the item command type prepare targetting
    switch(itemSchema.commandType)
    {
        case Item::CommandType::CommandTypeSelf:
        {
            // Send immediate command to use item on self
            Command cmd;
            cmd.type = Command::Type::ItemCommand;
            cmd.pack = [itemSchema] (sf::Packet &packet) {
                packet << itemSchema;
            };
            issueCommand(cmd);

            // Notify callback that the command has been issued
            if(onComplete)
            {
                onComplete();
            }

            break;
        }

        case Item::CommandType::CommandTypeCharacter:
        {
            StateGamePlay* state = this;
            // Enter character targetting mode and when a character is clicked run this
            levelClient->enterCharacterSelectMode(itemSchema.effectiveRangeInTiles, [state, itemSchema, onComplete] (CharacterClient* character, TileClient* tileClient) {
                Item::Schema itemSchemaX = itemSchema;

                Command cmd;
                cmd.type = Command::Type::ItemCommand;
                cmd.pack = [itemSchemaX, tileClient] (sf::Packet &packet) {
                    packet << itemSchemaX;
                    packet << sf::Uint32(tileClient->schema.id);
                };
                state->issueCommand(cmd);

                // Notify callback that the command has been issued
                if(onComplete)
                {
                    onComplete();
                }
            });
            break;
        }

        case Item::CommandType::CommandTypeArea:
        {
            StateGamePlay* state = this;
            // Enter area targetting mode and when a tile is clicked run this
            levelClient->enterAreaSelectMode(itemSchema.effectiveRangeInTiles, itemSchema.effectiveRadiusInTiles, [state, itemSchema, onComplete] (TileClient* tileClient) {
                Item::Schema itemSchemaX = itemSchema;

                Command cmd;
                cmd.type = Command::Type::ItemCommand;
                cmd.pack = [itemSchemaX, tileClient] (sf::Packet &packet) {
                    packet << itemSchemaX;
                    packet << sf::Uint32(tileClient->schema.id);
                };
                state->issueCommand(cmd);

                // Notify callback that the command has been issued
                if(onComplete)
                {
                    onComplete();
                }
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

        // Global input checking
        captureModelessInput(gameTime);

        // Set input variables for mode
        modeCaptureInput[mode](gameTime);
    }

    return true;
}

void StateGamePlay::captureModelessInput(sf::Time &gameTime)
{
    // This is input that is captured regardless of mode
    isShiftModifierDown = rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::LShift);
}

bool StateGamePlay::update(sf::Time &gameTime)
{
    bit::ClientServerState::update(gameTime);

    fps.update(gameTime);

    // Clear our state that our player has interacted with HUD
    isHudInteractionOccurring = false;

    // Update the HUD and mark if interaction has occurred (run before world events to make sure we dont have players clicking on some stuff and it triggering tile clicks and stuff)
    hud->update(*rogueZombieGame->renderWindow, gameTime);

    // Update the joining menu
    joiningMenu->update(*rogueZombieGame->renderWindow, gameTime);

    // Update gameplay mode input
    modeUpdate[mode](gameTime);

    levelClient->update(gameTime);

    // Camera
    if(levelClient->playerCharacter)
    {
        float zoomRatio = cameras[0]->getZoomRatio();
        float toleranceX = 250 * rogueZombieGame->currentResolutionRatioX / zoomRatio;
        float toleranceY = 150 * rogueZombieGame->currentResolutionRatioY / zoomRatio;

        sf::Vector2f position(levelClient->playerCharacter->BodyClient::schema.x, levelClient->playerCharacter->BodyClient::schema.y);
        position = bit::VectorMath::normalToIsometric(position.x, position.y);
        cameras[0]->lock(position.x, position.y, toleranceX, toleranceY, 8.0, 100.0);
    }

    return true;
}

void StateGamePlay::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::ClientServerState::draw(window, gameTime);

    rogueZombieGame->bgColor = sf::Color::Black;

    hud->draw(window, gameTime);
    joiningMenu->draw(window, gameTime);

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

void StateGamePlay::displayMessage(std::string const&message)
{
    hud->displayMessage(message);
}

void StateGamePlay::displayPlayerMessage(CharacterClient* character, std::string const&message)
{
    if(character == levelClient->playerCharacter)
    {
        displayMessage(message);
    }
}

TileClient* StateGamePlay::findNextInteractableTile()
{
    // we will be checking adjacent tiles in a clockwise motion
    // when one is found we return it and record the position it was last checked at
    // on a followup call we start at this position
    
    // get our players position
    if (levelClient->playerCharacter)
    {
        float x, y;
        x = levelClient->playerCharacter->BodyClient::schema.x;
        y = levelClient->playerCharacter->BodyClient::schema.y;

        // Check next directions
        for (unsigned int i = 0; i < interactionDirectionChecks.size(); i++)
        {
            // Update our index check to be the next direction (wrapping around)
            lastInteractionDirectionCheckIndex = (lastInteractionDirectionCheckIndex + 1) % interactionDirectionChecks.size();

            float worldX = x + (levelClient->tileWidth * interactionDirectionChecks[lastInteractionDirectionCheckIndex].x);
            float worldY = y + (levelClient->tileHeight * interactionDirectionChecks[lastInteractionDirectionCheckIndex].y);

            TileClient* t = levelClient->getTileAtWorldPosition(worldX, worldY);
            if (t && t->hasInteractableBody())
                return t;
        }
    }

    return NULL;
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
                displayMessage(std::string("Open Inventory is occupied"));
            }

            break;
        }
        case Interaction::Type::GetInventory:
        {
            bool success;
            packet >> success;

            if(success)
            {
                if(mode != Mode::Loot)
                {
                    changeMode(Mode::Loot); // this should not hit because this event is only issued after OpenInventory
                }
                hud->lootMenu->handleInventorySnapshot(packet, tileId);
            }
            else
            {
                displayMessage(std::string("Get Inventory is occupied"));
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
        case Interaction::Type::Consume:
        {
            bool success;
            packet >> success;

            if (success)
            {
                displayMessage(std::string("Body consumed"));
                changeMode(Mode::Free);
            }
            else
            {
                displayMessage(std::string("Body cannot be consumed"));
            }

            break;
        }
        case Interaction::Type::Destroy:
        {
            bool success;
            packet >> success;

            if (success)
            {
                displayMessage(std::string("Structure destroyed"));
                changeMode(Mode::Free);
            }
            else
            {
                displayMessage(std::string("Structure cannot be consumed"));
            }

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
    hud->onLevelChange(levelClient->levelId, levelId);
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

    if(levelClient->playerCharacter)
    {
        switch(eventType)
        {
            case ServerEvent::LeavingLevel:
                // Do not display anything to the player, we are 99% likely to show the arrival
                break;
            case ServerEvent::ArrivedLevel:
            {
                unsigned int levelId;
                packet >> levelId;
                switchLevels(levelId);
                levelClient->handleSnapshot(packet, true);
                std::stringstream ss; ss << "Arrived at level " << levelId;
                displayMessage(ss.str());
                break;
            }
            case ServerEvent::CannotTransitionNoRoom:
                displayMessage(std::string("Cannot leave level, exit is blocked"));
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
                hud->inventory->buildItemList(false);
                break;
            case ServerEvent::ItemAdded:
                levelClient->playerCharacter->handleServerEventPacket_itemAdded(packet);
                hud->inventory->buildItemList(false);
                break;
            case ServerEvent::ItemUpdated:
                levelClient->playerCharacter->handleServerEventPacket_itemUpdated(packet);
                hud->inventory->buildItemList(false);
                break;
            case ServerEvent::ItemRemoved:
                levelClient->playerCharacter->handleServerEventPacket_itemRemoved(packet);
                hud->inventory->buildItemList(false);
                break;
            case ServerEvent::EquipmentAdded:
                levelClient->playerCharacter->handleServerEventPacket_equipmentAdded(packet);
                hud->inventory->buildItemList(false);
                break;
            case ServerEvent::EquipmentUpdated:
                levelClient->playerCharacter->handleServerEventPacket_equipmentUpdated(packet);
                hud->inventory->buildItemList(false);
                break;
            case ServerEvent::EquipmentRemoved:
                levelClient->playerCharacter->handleServerEventPacket_equipmentRemoved(packet);
                hud->inventory->buildItemList(false);
                break;
            case ServerEvent::EquipmentSwapped:
                levelClient->playerCharacter->handleServerEventPacket_equipmentSwapped(packet);
                hud->inventory->buildItemList(false);
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

            case ServerEvent::Explosion:
            {
                float posX, posY;
                packet >> posX;
                packet >> posY;
                rogueZombieGame->soundManager->play(explosionSoundId);
                break;
            }

            case ServerEvent::Gunfire:
            {
                float posX, posY;
                Item::Type type;
                packet >> posX;
                packet >> posY;
                bit::NetworkHelper::unpackEnum<sf::Uint32, Item::Type>(packet, type);
                switch(type)
                {
                    case Item::Type::Z4Rifle:
                        rogueZombieGame->soundManager->play(rifleGunshotSoundId);
                        break;
                    case Item::Type::Magnum357:
                        rogueZombieGame->soundManager->play(pistolGunshotSoundId);
                        break;
                    default:
                        rogueZombieGame->soundManager->play(pistolGunshotSoundId);
                        break;
                }
                break;
            }

            case ServerEvent::DoorOpen:
            {
                float posX, posY;
                packet >> posX;
                packet >> posY;
                rogueZombieGame->soundManager->play(doorCloseSoundId);
                break;
            }

            case ServerEvent::DoorClose:
            {
                float posX, posY;
                packet >> posX;
                packet >> posY;
                rogueZombieGame->soundManager->play(doorCloseSoundId);
                break;
            }

            case ServerEvent::ChestOpen:
            {
                rogueZombieGame->soundManager->play(openChestSoundId);
                break;
            }

            case ServerEvent::ChestClose:
            {
                rogueZombieGame->soundManager->play(closeChestSoundId);
                break;
            }

            case ServerEvent::BodyOpen:
            {
                rogueZombieGame->soundManager->play(openBodySoundId);
                break;
            }

            case ServerEvent::BodyClose:
            {
                rogueZombieGame->soundManager->play(closeBodySoundId);
                break;
            }

            case ServerEvent::CharacterDeath:
            {
                float posX, posY;
                Character::Type type;
                packet >> posX;
                packet >> posY;
                bit::NetworkHelper::unpackEnum<sf::Uint32, Character::Type>(packet, type);
                switch(type)
                {
                    case Character::Type::Zombie:
                    case Character::Type::Hunter:
                    case Character::Type::Scientist:
                    case Character::Type::Guard:
                    case Character::Type::Hazmaster:
                        rogueZombieGame->soundManager->play(humanCharacterDeathSoundId);
                        break;
                }
                break;
            }

            case ServerEvent::BluntHit:
            {
                float posX, posY;
                packet >> posX;
                packet >> posY;
                rogueZombieGame->soundManager->play(thudSoundId);
                break;
            }

            case ServerEvent::BluntMiss:
            {
                float posX, posY;
                packet >> posX;
                packet >> posY;
                rogueZombieGame->soundManager->play(whiffSoundId);
                break;
            }

            case ServerEvent::CharacterHeal:
            {
                float posX, posY;
                packet >> posX;
                packet >> posY;
                rogueZombieGame->soundManager->play(healSoundId);
                levelClient->visualEffect(VisualEffect::Heal, posX, posY);
                break;
            }

            case ServerEvent::CharacterConsumed:
            {
                rogueZombieGame->soundManager->play(openBodySoundId);
                break;
            }

            case ServerEvent::StructureDestroyed:
            {
                rogueZombieGame->soundManager->play(thudSoundId);
                break;
            }

            case ServerEvent::TrapSprung:
            {
                float posX, posY;
                packet >> posX;
                packet >> posY;
                rogueZombieGame->soundManager->play(spikeReleaseSoundId);
                displayMessage("Trap sprung!");
                break;
            }

            case ServerEvent::TrapReset:
            {
                float posX, posY;
                packet >> posX;
                packet >> posY;
                rogueZombieGame->soundManager->play(spikeReturnSoundId);
                break;
            }

            case ServerEvent::CharacterEffectCreated:
            {
                CharacterEffect::Type type;
                unsigned int characterId;
                float posX, posY;
                bit::NetworkHelper::unpackEnum<sf::Uint32, CharacterEffect::Type>(packet, type);
                packet >> characterId;
                packet >> posX;
                packet >> posY;
                CharacterClient* cc = levelClient->getCharacterById(characterId);
                if (cc)
                {
                    cc->onEffectCreate(type);
                }
                break;
            }

            case ServerEvent::CharacterEffectRun:
            {
                CharacterEffect::Type type;
                unsigned int characterId;
                float posX, posY;
                bit::NetworkHelper::unpackEnum<sf::Uint32, CharacterEffect::Type>(packet, type);
                packet >> characterId;
                packet >> posX;
                packet >> posY;
                CharacterClient* cc = levelClient->getCharacterById(characterId);
                if (cc)
                {
                    cc->onEffectRun(type);
                }
                break;
            }

            case ServerEvent::CharacterEffectRemoved:
            {
                CharacterEffect::Type type;
                unsigned int characterId;
                float posX, posY;
                bit::NetworkHelper::unpackEnum<sf::Uint32, CharacterEffect::Type>(packet, type);
                packet >> characterId;
                packet >> posX;
                packet >> posY;
                CharacterClient* cc = levelClient->getCharacterById(characterId);
                if (cc)
                {
                    cc->onEffectRemove(type);
                }
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