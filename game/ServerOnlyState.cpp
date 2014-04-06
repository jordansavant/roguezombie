#include "ServerOnlyState.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "RogueZombieServer.hpp"
#include "GameplayServer.hpp"

ServerOnlyState::ServerOnlyState(bit::StateStack &stack, RogueZombieServer* _game)
    : bit::ClientServerState(stack, _game, false, true), rogueZombieServer(_game)
{
}

ServerOnlyState::~ServerOnlyState()
{
}

void ServerOnlyState::load()
{
    bit::ClientServerState::load();
}

bool ServerOnlyState::update(sf::Time &gameTime)
{
    bit::ClientServerState::update(gameTime);
    
    return true;
}

bit::Server* ServerOnlyState::newServer()
{
    return new GameplayServer();
}

std::string ServerOnlyState::getServerIpAddress()
{
    return "192.168.0.100";
}

/**
 * Packet handling
 */

void ServerOnlyState::handlePacket_Broadcast(bit::ServerPacket &packet)
{
}

void ServerOnlyState::handlePacket_InitializeSelf(bit::ServerPacket &packet)
{
}

void ServerOnlyState::handlePacket_InitializeWorld(bit::ServerPacket &packet)
{
}

void ServerOnlyState::handlePacket_PeerClientConnected(bit::ServerPacket &packet)
{
}

void ServerOnlyState::handlePacket_ClientDisonnected(bit::ServerPacket &packet)
{
}

void ServerOnlyState::handlePacket_ServerUpdate(bit::ServerPacket &packet)
{
}

void ServerOnlyState::handlePacket_Shutdown(bit::ServerPacket &packet)
{
}

/**
 * Packet sending
 */

void ServerOnlyState::preparePacket_ClientInformation(bit::ClientPacket &packet)
{
}

void ServerOnlyState::preparePacket_ClientUpdate(bit::ClientPacket &packet)
{
}
