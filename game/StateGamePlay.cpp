#include "StateGamePlay.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/System.hpp"
#include "../ResourcePath.h"
#include "RogueZombieGame.hpp"
#include "GameplayServer.hpp"
#include "ServerEvent.hpp"
#include "ClientRequest.hpp"
#include "LevelClient.hpp"
#include "TileClient.hpp"
#include "Command.hpp"
#include "characters/ZombieClient.hpp"
#include "hud/Hud.hpp"
#include "hud/InteractionMenu.hpp"
#include "hud/LootMenu.hpp"
#include "items/ItemClient.hpp"
#include <sstream>

StateGamePlay::StateGamePlay(bit::StateStack &stack, RogueZombieGame* _game, bool isClient, bool isHost)
    : bit::ClientServerState(stack, _game, isClient, isHost), rogueZombieGame(_game), mode(Mode::Free), fps()
{
    std::string fpsFontPath(resourcePath() + "Agency.ttf");
    fps.load(fpsFontPath, 10, 10);
    fps.fpsText.setColor(sf::Color(50, 50, 50));
    createCamera(rogueZombieGame, 0, 0, 1, 1);
    cameras[0]->panSpeed = 3;
    levelClient = new LevelClient();

    hud = new Hud(this);

    // Game play mode logic
    modeEnter.resize(Mode::_count, NULL);
    modeExit.resize(Mode::_count, NULL);
    modeUpdate.resize(Mode::_count, NULL);

    modeEnter[Mode::Free] = std::bind(&StateGamePlay::modeOnEnterFree, this);
    modeExit[Mode::Free] = std::bind(&StateGamePlay::modeOnExitFree, this);
    modeUpdate[Mode::Free] = std::bind(&StateGamePlay::modeOnUpdateFree, this, std::placeholders::_1);
    
    modeEnter[Mode::Loot] = std::bind(&StateGamePlay::modeOnEnterLoot, this);
    modeExit[Mode::Loot] = std::bind(&StateGamePlay::modeOnExitLoot, this);
    modeUpdate[Mode::Loot] = std::bind(&StateGamePlay::modeOnUpdateLoot, this, std::placeholders::_1);
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

void StateGamePlay::changeMode(Mode _mode)
{
    if(_mode != mode)
    {
        displayMessage(std::string("Changed modes"));
        modeExit[mode]();
        mode = _mode;
        modeEnter[mode]();
    }
}


////////////////////////////////////////////////////////////////////////
//                            FREE MODES                              //
////////////////////////////////////////////////////////////////////////

void StateGamePlay::modeOnEnterFree()
{
    hud->hideAllMenus();
}

void StateGamePlay::modeOnExitFree()
{
    hud->interactionMenu->deactivate();
}

void StateGamePlay::modeOnUpdateFree(sf::Time &gameTime)
{
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Up))
        cameras[0]->direction.y = -1;
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Down))
        cameras[0]->direction.y = 1;
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Left))
        cameras[0]->direction.x = -1;
    if(rogueZombieGame->inputManager->isButtonDown(sf::Keyboard::Right))
        cameras[0]->direction.x = 1;

    // Listen for Game Commands
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::X))
    {
	    Command cmd;
        cmd.type = Command::Type::PlayerSwitchLevel;
	    commandQueue.push_back(cmd);
    }
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::W))
    {
        Command cmd;
        cmd.type = Command::Type::PlayerMoveUp;
	    commandQueue.push_back(cmd);
    }
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::S))
    {
	    Command cmd;
        cmd.type = Command::Type::PlayerMoveDown;
	    commandQueue.push_back(cmd);
    }
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::A))
    {
	    Command cmd;
        cmd.type = Command::Type::PlayerMoveLeft;
	    commandQueue.push_back(cmd);
    }
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::D))
    {
	    Command cmd;
        cmd.type = Command::Type::PlayerMoveRight;
	    commandQueue.push_back(cmd);
    }
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
                else
                    displayMessage(std::string("Object is too far away"));
            }
            // Else issue command to move to tile
            else
            {
                Command cmd;
                cmd.type = Command::Type::MoveToTile;
                cmd.pack = [t] (sf::Packet &packet) {
                    packet << sf::Uint32(t->schema.id);
                };
		        commandQueue.push_back(cmd);
            }
        }
    }
    if(rogueZombieGame->inputManager->isButtonReleased(sf::Mouse::Right))
    {
        // See if a tile is being hovered over
        if(levelClient->hoveredTile)
        {
            TileClient* t = levelClient->hoveredTile;
            Command cmd;
            cmd.type = Command::Type::PlayerRightClickTile;
            cmd.pack = [t] (sf::Packet &packet) {
                packet << sf::Uint32(t->schema.id);
            };
		    commandQueue.push_back(cmd);
        }
    }

    
    // Inentory hot key
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::I))
    {
        hud->activateInventory(true);
    }
    // Journal hot key
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::J))
    {
        hud->activateJournal(true);
    }

    // Exit
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        disconnect();
    }
}


////////////////////////////////////////////////////////////////////////
//                            LOOT MODES                              //
////////////////////////////////////////////////////////////////////////

void StateGamePlay::modeOnEnterLoot()
{
    hud->activateInventory(false);
    hud->lootMenu->activate();
}

void StateGamePlay::modeOnExitLoot()
{
    hud->lootMenu->deactivate();

    serverRequest(
        [](bit::ClientPacket& packet) {
            packet << sf::Uint32(ClientRequest::CloseInventory);
        },
        [](bit::ServerPacket& response){
        }
    );
}

void StateGamePlay::modeOnUpdateLoot(sf::Time &gameTime)
{
	// Exit
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        hud->lootMenu->deactivate();
        changeMode(Mode::Free);
    }
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

    // Test journal
    

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
            packet << sf::Uint32(ClientRequest::GetInteractionOptions);
            packet << sf::Uint32(tileId);
        },
        [tileId, s] (bit::ServerPacket& packet) // onComplete
        {
            bit::Output::Debug("Client request interaction options received");
            s->hud->interactionMenu->handleInteractionTree(packet, tileId);
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
    }
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
    levelClient->load(this);
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
        delete levelClient;
        levelClient = new LevelClient();
        levelClient->levelId = levelId;
        levelClient->load(this);
    }

    if(isFullSnapshot) bit::Output::Debug("received full");

    levelClient->handleSnapshot(packet, isFullSnapshot);
}

void StateGamePlay::handlePacket_ServerEvent(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle server event");

    unsigned int eventTypeInt;
    packet >> eventTypeInt;
    ServerEvent eventType = static_cast<ServerEvent>(eventTypeInt);

    if(levelClient->playerCharacter)
    {
        switch(eventType)
        {
            case ServerEvent::MissionCompleted:
                levelClient->playerCharacter->handleServerEventPacket_missionCompleted(packet);
                break;
            case ServerEvent::ItemAdded:
                levelClient->playerCharacter->handleServerEventPacket_itemAdded(packet);
                break;
        }
    }
}

void StateGamePlay::handlePacket_DisconnectAcknowledge(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle client disconnected acknowledged");

    // Exit
    requestStackPop();
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
        packet << static_cast<sf::Uint8>(commandQueue[i].type);
        commandQueue[i].pack(packet);
	}

	// Clear commands
	commandQueue.clear();
}

void StateGamePlay::preparePacket_ClientDisconnect(bit::ClientPacket &packet)
{
    bit::Output::Debug("Client prepare client disconnect");
}