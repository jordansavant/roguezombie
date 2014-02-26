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

    clientWorld.load();
}


bool MultiplayerState::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::ClientServerState::update(window, gameTime);

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

    bit::ClientServerState::handlePacket_Broadcast(packet);
}

void MultiplayerState::handlePacket_InitializeSelf(sf::Packet &packet)
{
    bit::Output::Debug("Client handle initialize self");

    bit::ClientServerState::handlePacket_InitializeSelf(packet);
}

void MultiplayerState::handlePacket_PeerClientConnected(sf::Packet &packet) 
{
    bit::Output::Debug("Client handle client connected");

    bit::ClientServerState::handlePacket_PeerClientConnected(packet);
}

void MultiplayerState::handlePacket_ClientDisonnected(sf::Packet &packet)
{
    bit::Output::Debug("Client handle client disconnected");

    bit::ClientServerState::handlePacket_ClientDisonnected(packet);
}

void MultiplayerState::handlePacket_ServerUpdate(sf::Packet &packet)
{
    bit::Output::Debug("Client handle server update");

    clientWorld.extractSnapshot(packet);

    bit::ClientServerState::handlePacket_ServerUpdate(packet);
}

void MultiplayerState::handlePacket_Shutdown(sf::Packet &packet)
{
    bit::Output::Debug("Client handle server shutdown");

    bit::ClientServerState::handlePacket_Shutdown(packet);
}


/**
 * Packet sending
 */

sf::Packet& MultiplayerState::preparePacket_ClientInformation(sf::Packet &packet)
{
    bit::Output::Debug("Client prepare client information");

    return bit::ClientServerState::preparePacket_ClientInformation(packet);
}

sf::Packet& MultiplayerState::preparePacket_ClientUpdate(sf::Packet &packet)
{
    bit::Output::Debug("Client prepare client update");

    return bit::ClientServerState::preparePacket_ClientUpdate(packet);
}