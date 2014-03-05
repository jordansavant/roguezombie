#include "GameplayState.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "../bitengine/Input.hpp"
#include "../bitengine/Graphics.hpp"
#include "../bitengine/System.hpp"
#include "../ResourcePath.h"
#include "GameplayServer.hpp"
#include "WorldClient.hpp"
#include "Command.hpp"
#include "characters/ZombieClient.hpp"
#include <sstream>

GameplayState::GameplayState(bit::StateStack &stack, bit::Game* _game, bool isHost)
    : bit::ClientServerState(stack, _game, isHost)
{
    createCamera(*game->renderWindow, 0, 0, 1, 1);
    cameras[0]->lockOnPoint(0, 160);
}

void GameplayState::load()
{
    bit::ClientServerState::load();

    worldClient.load(this);
}

bool GameplayState::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::ClientServerState::update(window, gameTime);

	// Listen for Game Commands
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
    if(game->inputManager->isButtonPressed(sf::Keyboard::T))
	{
		Command cmd;
        cmd.type = Command::Type::PlayerTeleport;
        cmd.pack = [] (sf::Packet &packet) {
            packet << 0.0f << 0.0f;
        };
		commandQueue.push_back(cmd);
	}

	// Exit
    if(game->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        requestStateClear();
    }

    worldClient.update(window, gameTime);

    return true;
}

void GameplayState::drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera)
{
    worldClient.draw(window, gameTime);
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

    worldClient.handleSnapshot(packet, true);
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

    worldClient.handleSnapshot(packet);
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

    if(commandCount == 1)
    {
        int fooop = 0;
    }

	for(unsigned int i=0; i < commandQueue.size(); i++)
	{
		// Push commands onto packet, oldest first
        packet << static_cast<sf::Uint8>(commandQueue[i].type);
        commandQueue[i].pack(packet);
	}

	// Clear commands
	commandQueue.clear();
}