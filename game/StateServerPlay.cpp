#include "StateServerPlay.hpp"
#include "../bitengine/Game.hpp"
#include "../bitengine/Network.hpp"
#include "RogueZombieServer.hpp"
#include "GameplayServer.hpp"

StateServerPlay::StateServerPlay(bit::StateStack &stack, RogueZombieServer* _game)
    : bit::ClientServerState(stack, _game, false, true), rogueZombieServer(_game)
{
}

StateServerPlay::~StateServerPlay()
{
}

void StateServerPlay::load()
{
    bit::ClientServerState::load();
}

bool StateServerPlay::update(sf::Time &gameTime)
{
    bit::ClientServerState::update(gameTime);
    
    return true;
}

bit::Server* StateServerPlay::newServer()
{
    return new GameplayServer();
}

std::string StateServerPlay::getServerIpAddress()
{
    return "192.168.0.100";
}

/**
 * Packet handling
 */

void StateServerPlay::handlePacket_Broadcast(bit::ServerPacket &packet)
{
}

void StateServerPlay::handlePacket_InitializeSelf(bit::ServerPacket &packet)
{
}

void StateServerPlay::handlePacket_InitializeWorld(bit::ServerPacket &packet)
{
}

void StateServerPlay::handlePacket_PeerClientConnected(bit::ServerPacket &packet)
{
}

void StateServerPlay::handlePacket_ClientDisonnected(bit::ServerPacket &packet)
{
}

void StateServerPlay::handlePacket_ServerUpdate(bit::ServerPacket &packet)
{
}

void StateServerPlay::handlePacket_Shutdown(bit::ServerPacket &packet)
{
}

/**
 * Packet sending
 */

void StateServerPlay::preparePacket_ClientInformation(bit::ClientPacket &packet)
{
}

void StateServerPlay::preparePacket_ClientUpdate(bit::ClientPacket &packet)
{
}
