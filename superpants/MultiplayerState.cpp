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


MultiplayerState::MultiplayerState(bit::StateStack &stack, bit::Game* _game, bool isHost)
    : bit::ClientServerState(stack, _game, isHost)
{
    createCamera(*game->renderWindow, 0, 0, 1, 1);
    this->cameras[0]->lockOnPoint(200, 200);
}


void MultiplayerState::load()
{
    bit::ClientServerState::load();

    clientWorld.clientLoad();
}


bool MultiplayerState::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::ClientServerState::update(window, gameTime);

    if(game->inputManager->isButtonPressed(sf::Keyboard::Escape))
    {
        requestStateClear();
    }

    clientWorld.clientUpdate(gameTime);

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
    clientWorld.extractSnapshot(packet);
    bit::Output::Debug("Client handle server update");
}

void MultiplayerState::handlePacket_Shutdown(sf::Packet &packet)
{
    bit::Output::Debug("Client handle server shutdown");
}



sf::Packet& MultiplayerState::preparePacket_ClientUpdate(sf::Packet &packet)
{
    bit::Output::Debug("Client prepare client update");
    return packet;
}