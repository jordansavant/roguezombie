#include "StateGamePlay.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/System.hpp"
#include "../ResourcePath.h"
#include "RogueZombieGame.hpp"
#include "GameplayServer.hpp"
#include "LevelClient.hpp"
#include "TileClient.hpp"
#include "Command.hpp"
#include "characters/ZombieClient.hpp"
#include "hud/OptionsBar.hpp"
#include <sstream>

StateGamePlay::StateGamePlay(bit::StateStack &stack, RogueZombieGame* _game, bool isClient, bool isHost)
    : bit::ClientServerState(stack, _game, isClient, isHost), rogueZombieGame(_game), fps()
{
    std::string fpsFontPath(resourcePath() + "Agency.ttf");
    fps.load(fpsFontPath, 10, 10);
    createCamera(rogueZombieGame, 0, 0, 1, 1);
    cameras[0]->panSpeed = 3;
    levelClient = new LevelClient();

    // Test Gui
    journalFont.loadFromFile(resourcePath() + "homespun.ttf");
    journal = new bit::Container(0, 50, 300, 200, bit::Element::AnchorType::TopRight);
    journalEntries = new bit::Label(0, 0, 0, 0, bit::Element::AnchorType::TopLeft);
    journalEntries->setSfFontSize(24);
    journalEntries->setSfFont(journalFont);
    journalEntries->normalColor = sf::Color::White;
    journal->addChild(journalEntries);

    rogueZombieGame->spriteLoader->loadSprites(resourcePath() + "interface_01.csv");
    interfaceTexture.loadFromFile(resourcePath() + "interface_01.png");
    interfaceVertexMap.load(&interfaceTexture, sf::PrimitiveType::Quads);

    optionsBar = new OptionsBar(this);
}

StateGamePlay::~StateGamePlay()
{
    delete journal;
    delete optionsBar;
    delete levelClient;
}

void StateGamePlay::load()
{
    bit::ClientServerState::load();

    levelClient->load(this);
}

bool StateGamePlay::update(sf::Time &gameTime)
{
    bit::ClientServerState::update(gameTime);

    fps.update(gameTime);

    // Test journal
    if(levelClient->playerCharacter)
    {
        std::string entry("Missions\n");
        for(unsigned int i=0; i < levelClient->playerCharacter->missionClients.size(); i++)
        {
            for(unsigned int j=0; j < levelClient->playerCharacter->missionClients[i].requirements.size(); j++)
            {
                RequirementClient* rc = &levelClient->playerCharacter->missionClients[i].requirements[j];
                if(rc->isFullfilled)
                    entry += "- " + rc->journalEntry.title + " - Success\n";
                else
                    entry += "- " + rc->journalEntry.title + "\n";
            }
        }
        journalEntries->setSfFontString(entry);
        journal->update(*rogueZombieGame->renderWindow, gameTime);
    }

    optionsBar->update(*rogueZombieGame->renderWindow, gameTime);

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
            Command cmd;
            cmd.type = Command::Type::PlayerClickTile;
            cmd.pack = [t] (sf::Packet &packet) {
                packet << sf::Uint32(t->fixedState.id);
            };
		    commandQueue.push_back(cmd);
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
                packet << sf::Uint32(t->fixedState.id);
            };
		    commandQueue.push_back(cmd);
        }
    }

	// Exit
    if(rogueZombieGame->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        disconnect();
    }

    levelClient->update(gameTime);

    levelClient->minimap.setPosition(150 * rogueZombieGame->currentResolutionRatioX, 100 * rogueZombieGame->currentResolutionRatioY);
    levelClient->minimap.setScale(rogueZombieGame->currentResolutionRatio, rogueZombieGame->currentResolutionRatio);

    // Camera
    if(levelClient->playerCharacter)
    {
        float toleranceX = 250 * rogueZombieGame->currentResolutionRatioX;
        float toleranceY = 150 * rogueZombieGame->currentResolutionRatioY;

        sf::Vector2f position(levelClient->playerCharacter->Body::deltaState.x, levelClient->playerCharacter->Body::deltaState.y);
        position = bit::VectorMath::normalToIsometric(position.x, position.y);
        cameras[0]->lock(position.x, position.y, toleranceX, toleranceY, 4.0, 100.0);
    }

    return true;
}

void StateGamePlay::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::ClientServerState::draw(window, gameTime);

    window.draw(levelClient->minimap);

    journal->draw(window, gameTime);
    optionsBar->draw(window, gameTime);

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