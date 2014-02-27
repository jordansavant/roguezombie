#include "MultiplayerState.hpp"
#include "../bitengine/Game/Game.hpp"
#include "../bitengine/Game/ClientServerState.hpp"
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
		commandQueue.push_back(Command::PlayerUp);
	}
	if(game->inputManager->isButtonDown(sf::Keyboard::S))
	{
		commandQueue.push_back(Command::PlayerDown);
	}
	if(game->inputManager->isButtonDown(sf::Keyboard::A))
	{
		commandQueue.push_back(Command::PlayerLeft);
	}
	if(game->inputManager->isButtonDown(sf::Keyboard::D))
	{
		commandQueue.push_back(Command::PlayerRight);
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


/**
 * Packet handling
 */

void MultiplayerState::handlePacket_Broadcast(sf::Packet &packet)
{
    bit::Output::Debug("Client handle broadcast");
}

void MultiplayerState::handlePacket_InitializeSelf(sf::Packet &packet)
{
    bit::Output::Debug("Client handle initialize self");
}

void MultiplayerState::handlePacket_PeerClientConnected(sf::Packet &packet) 
{
    bit::Output::Debug("Client handle client connected");
}

void MultiplayerState::handlePacket_ClientDisonnected(sf::Packet &packet)
{
    bit::Output::Debug("Client handle client disconnected");
}

void MultiplayerState::handlePacket_ServerUpdate(sf::Packet &packet)
{
    bit::Output::Debug("Client handle server update");

    clientWorld.extractSnapshot(packet);
}

void MultiplayerState::handlePacket_Shutdown(sf::Packet &packet)
{
    bit::Output::Debug("Client handle server shutdown");
}


/**
 * Packet sending
 */

sf::Packet& MultiplayerState::preparePacket_ClientInformation(sf::Packet &packet)
{
    bit::Output::Debug("Client prepare client information");

    return packet;
}

sf::Packet& MultiplayerState::preparePacket_ClientUpdate(sf::Packet &packet)
{
    bit::Output::Debug("Client prepare client update");

	// Notify of command count
	sf::Uint32 commandCount;
	commandCount = commandQueue.size();
	packet << commandCount;

	for(unsigned int i=0; i < commandQueue.size(); i++)
	{
		// Push commands onto packet, oldest first
		packet << static_cast<sf::Int32>(commandQueue[i]);
	}

	// Clear commands
	commandQueue.clear();

    return packet;
}