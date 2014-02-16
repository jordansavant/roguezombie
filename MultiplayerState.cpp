#include "MultiplayerState.hpp"
#include "bitengine/Game/Game.hpp"
#include "bitengine/Game/ClientServerState.hpp"
#include "bitengine/Game/StateStack.hpp"
#include "bitengine/Input/InputManager.hpp"
#include "bitengine/Graphics/Camera.hpp"
#include "ResourcePath.h"
#include "TestServer.hpp"
#include "bitengine/System/Output.hpp"


MultiplayerState::MultiplayerState(bit::StateStack &stack, bit::Game* _game, bool isHost)
    : bit::ClientServerState(stack, _game, isHost), shape(200)
{
    if(!isHost)
    {
        zombieimage.loadFromFile(resourcePath() + "Zombie.png");
        zombiesprite.setTexture(zombieimage);

        createCamera(*game->renderWindow, 0, 0, 1, 1);
    }
}


bool MultiplayerState::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    if(game->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        requestStateClear();
    }

    return true;
}


void MultiplayerState::draw(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::State::draw(window, gameTime);
}


void MultiplayerState::drawForCamera(sf::RenderWindow &window, sf::Time &gameTime, bit::Camera &camera)
{
	window.draw(shape);
    window.draw(zombiesprite);
}


bit::Server* MultiplayerState::newServer()
{
    return new TestServer();
}

void MultiplayerState::handlePacket_Broadcast(sf::Packet &packet)
{
    bit::Output::Debug("Client handle broadcast");
}

void MultiplayerState::handlePacket_InitializeSelf(sf::Packet &packet)
{
    bit::Output::Debug("Client handle initialize self");
}

void MultiplayerState::handlePacket_InitializeWorld(sf::Packet &packet)
{
    bit::Output::Debug("Client handle initialize world");
}

void MultiplayerState::handlePacket_PeerConnected(sf::Packet &packet) 
{
    bit::Output::Debug("Client handle peer connected");
}

void MultiplayerState::handlePacket_PeerDisonnected(sf::Packet &packet)
{
    bit::Output::Debug("Client handle peer disconnected");
}

void MultiplayerState::handlePacket_Update(sf::Packet &packet)
{
    bit::Output::Debug("Client handle update");
}

void MultiplayerState::handlePacket_PeerEvent(sf::Packet &packet)
{
    bit::Output::Debug("Client handle peer event");
}

void MultiplayerState::handlePacket_PeerRealtimeChange(sf::Packet &packet)
{
    bit::Output::Debug("Client handle peer realtimechange");
}

void MultiplayerState::handlePacket_Shutdown(sf::Packet &packet)
{
    bit::Output::Debug("Client handle server shutdown");
}
