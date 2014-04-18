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
    return "127.0.0.1";
}

unsigned short StateServerPlay::getServerPort()
{
    return BIT_SERVER_PORT;
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

void StateServerPlay::handlePacket_ServerEvent(bit::ServerPacket &packet)
{
}

void StateServerPlay::handlePacket_DisconnectAcknowledge(bit::ServerPacket &packet)
{
}

void StateServerPlay::handlePacket_Shutdown(bit::ServerPacket &packet)
{
}

void StateServerPlay::handle_DisconnectTimeout()
{
}

void StateServerPlay::handlePacket_Kick(bit::ServerPacket &packet)
{
}

void StateServerPlay::handle_ServerTimeout()
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

void StateServerPlay::preparePacket_ClientDisconnect(bit::ClientPacket &packet)
{
}
