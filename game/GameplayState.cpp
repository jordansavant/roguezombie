#include "GameplayState.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/System.hpp"
#include "../ResourcePath.h"
#include "GameplayServer.hpp"
#include "LevelClient.hpp"
#include "TileClient.hpp"
#include "Command.hpp"
#include "characters/ZombieClient.hpp"
#include <sstream>

GameplayState::GameplayState(bit::StateStack &stack, bit::Game* _game, bool isHost)
    : bit::ClientServerState(stack, _game, isHost), fps()
{
    std::string fpsFontPath(resourcePath() + "Agency.ttf");
    fps.load(fpsFontPath, 10, 10);
    createCamera(*game->renderWindow, 0, 0, 1, 1);
    cameras[0]->panSpeed = 3;
    levelClient = new LevelClient();
}

GameplayState::~GameplayState()
{
    delete levelClient;
}

void GameplayState::load()
{
    bit::ClientServerState::load();
}

bool GameplayState::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::ClientServerState::update(window, gameTime);

    fps.update(gameTime);

    if(game->inputManager->isButtonDown(sf::Keyboard::Up))
        cameras[0]->direction.y = -1;
    if(game->inputManager->isButtonDown(sf::Keyboard::Down))
        cameras[0]->direction.y = 1;
    if(game->inputManager->isButtonDown(sf::Keyboard::Left))
        cameras[0]->direction.x = -1;
    if(game->inputManager->isButtonDown(sf::Keyboard::Right))
        cameras[0]->direction.x = 1;

	// Listen for Game Commands
    if(game->inputManager->isButtonPressed(sf::Keyboard::X))
	{
		Command cmd;
        cmd.type = Command::Type::PlayerSwitchLevel;
		commandQueue.push_back(cmd);
	}
    if(game->inputManager->isButtonPressed(sf::Keyboard::W))
	{
        Command cmd;
        cmd.type = Command::Type::PlayerMoveUp;
		commandQueue.push_back(cmd);
	}
	if(game->inputManager->isButtonPressed(sf::Keyboard::S))
	{
		Command cmd;
        cmd.type = Command::Type::PlayerMoveDown;
		commandQueue.push_back(cmd);
	}
	if(game->inputManager->isButtonPressed(sf::Keyboard::A))
	{
		Command cmd;
        cmd.type = Command::Type::PlayerMoveLeft;
		commandQueue.push_back(cmd);
	}
	if(game->inputManager->isButtonPressed(sf::Keyboard::D))
	{
		Command cmd;
        cmd.type = Command::Type::PlayerMoveRight;
		commandQueue.push_back(cmd);
	}
    if(game->inputManager->isButtonReleased(sf::Mouse::Left))
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
    if(game->inputManager->isButtonReleased(sf::Mouse::Right))
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
    if(game->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        requestStateClear();
    }

    levelClient->update(window, gameTime);
    
    levelClient->minimap.setPosition(150 * game->currentResolutionRatioX, 50 * game->currentResolutionRatioY);
    levelClient->minimap.setScale(game->currentResolutionRatio, game->currentResolutionRatio);

    // Camera
    if(levelClient->playerCharacter)
    {
        float toleranceX = 250 * game->currentResolutionRatioX;
        float toleranceY = 150 * game->currentResolutionRatioY;

        sf::Vector2f position(levelClient->playerCharacter->Body::deltaState.x, levelClient->playerCharacter->Body::deltaState.y);
        position = bit::VectorMath::normalToIsometric(position.x, position.y);
        cameras[0]->lock(position.x, position.y, toleranceX, toleranceY, 4.0, 100.0);
    }

    return true;
}

void GameplayState::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::ClientServerState::draw(window, gameTime);
    
    window.draw(levelClient->minimap);

    fps.draw(window, gameTime);
}

void GameplayState::drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera)
{
    window.draw(*levelClient);
}

bit::Server* GameplayState::newServer()
{
    return new GameplayServer();
}

std::string GameplayState::getServerIpAddress()
{
    return "192.168.0.100";
}

/**
 * Packet handling
 */

void GameplayState::handlePacket_Broadcast(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle broadcast");
}

void GameplayState::handlePacket_InitializeSelf(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle initialize self");
}

void GameplayState::handlePacket_InitializeWorld(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle initialize world");

    sf::Uint32 levelId;
    packet >> levelId;
    levelClient->levelId = levelId;
    levelClient->load(this);
    levelClient->handleSnapshot(packet, true);
}

void GameplayState::handlePacket_PeerClientConnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle client connected");
}

void GameplayState::handlePacket_ClientDisonnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle client disconnected");
}

void GameplayState::handlePacket_ServerUpdate(bit::ServerPacket &packet)
{
    //bit::Output::Debug("Client handle server update");

    sf::Uint32 levelId;
    packet >> levelId;
    if(levelClient->levelId != levelId)
    {
        bit::Output::Debug(levelId);
        delete levelClient;
        levelClient = new LevelClient();
        levelClient->levelId = levelId;
        levelClient->load(this);
    }

    levelClient->handleSnapshot(packet, true);
}

void GameplayState::handlePacket_Shutdown(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle server shutdown");
}

/**
 * Packet sending
 */

void GameplayState::preparePacket_ClientInformation(bit::ClientPacket &packet)
{
    bit::Output::Debug("Client prepare client information");
}

void GameplayState::preparePacket_ClientUpdate(bit::ClientPacket &packet)
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
