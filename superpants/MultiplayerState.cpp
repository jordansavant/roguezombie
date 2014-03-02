#include "MultiplayerState.hpp"
#include "../bitengine/Game/Game.hpp"
#include "../bitengine/Network/ClientServerState.hpp"
#include "../bitengine/Network/ClientPacket.hpp"
#include "../bitengine/Game/StateStack.hpp"
#include "../bitengine/Input/InputManager.hpp"
#include "../bitengine/Graphics/Camera.hpp"
#include "../bitengine/System/Output.hpp"
#include "../ResourcePath.h"
#include "TestServer.hpp"
#include "World.hpp"
#include "Command.hpp"

MultiplayerState::MultiplayerState(bit::StateStack &stack, bit::Game* _game, bool isHost)
    : bit::ClientServerState(stack, _game, isHost)
{
    createCamera(*game->renderWindow, 0, 0, 1, 1);
    this->cameras[0]->lockOnPoint(200, 200);
}

void MultiplayerState::load()
{
    bit::ClientServerState::load();

    clientWorld.load();
}

bool MultiplayerState::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::ClientServerState::update(window, gameTime);

	// Listen for Game Commands
	if(game->inputManager->isButtonDown(sf::Keyboard::W))
	{
        Command cmd;
        cmd.type = Command::Type::PlayerMoveUp;
		commandQueue.push_back(cmd);
	}
	if(game->inputManager->isButtonDown(sf::Keyboard::S))
	{
		Command cmd;
        cmd.type = Command::Type::PlayerMoveDown;
		commandQueue.push_back(cmd);
	}
	if(game->inputManager->isButtonDown(sf::Keyboard::A))
	{
		Command cmd;
        cmd.type = Command::Type::PlayerMoveLeft;
		commandQueue.push_back(cmd);
	}
	if(game->inputManager->isButtonDown(sf::Keyboard::D))
	{
		Command cmd;
        cmd.type = Command::Type::PlayerMoveRight;
		commandQueue.push_back(cmd);
	}
    if(game->inputManager->isButtonDown(sf::Keyboard::T))
	{
		Command cmd;
        cmd.type = Command::Type::PlayerTeleport;
        cmd.data << 20.0f << 30.0f;
		commandQueue.push_back(cmd);
	}

	// Exit
    if(game->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        requestStateClear();
    }

    clientWorld.update(gameTime);

    return true;
}

void MultiplayerState::drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera)
{
    clientWorld.draw(window, gameTime);
}

bit::Server* MultiplayerState::newServer()
{
    return new TestServer();
}

std::string MultiplayerState::getServerIpAddress()
{
    return "192.168.0.100";
}

/**
 * Packet handling
 */

void MultiplayerState::handlePacket_Broadcast(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle broadcast");
}

void MultiplayerState::handlePacket_InitializeSelf(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle initialize self");
}

void MultiplayerState::handlePacket_PeerClientConnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle client connected");
}

void MultiplayerState::handlePacket_ClientDisonnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle client disconnected");
}

void MultiplayerState::handlePacket_ServerUpdate(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle server update");

    clientWorld.handleSnapshot(packet);
}

void MultiplayerState::handlePacket_Shutdown(bit::ServerPacket &packet)
{
    bit::Output::Debug("Client handle server shutdown");
}

/**
 * Packet sending
 */

void MultiplayerState::preparePacket_ClientInformation(bit::ClientPacket &packet)
{
    bit::Output::Debug("Client prepare client information");
}

void MultiplayerState::preparePacket_ClientUpdate(bit::ClientPacket &packet)
{
    bit::Output::Debug("Client prepare client update");

	// Notify of command count
	sf::Uint32 commandCount;
	commandCount = commandQueue.size();
	packet << commandCount;

	for(unsigned int i=0; i < commandQueue.size(); i++)
	{
		// Push commands onto packet, oldest first
        packet << static_cast<sf::Int32>(commandQueue[i].type);

        if(commandQueue[i].data.getDataSize() > 0)
        {
            packet << commandQueue[i].data;
        }
	}

	// Clear commands
	commandQueue.clear();
}