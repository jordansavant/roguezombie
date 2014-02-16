#include "ClientServerState.hpp"
#include "State.hpp"
#include "StateStack.hpp"
#include "../Graphics/Camera.hpp"
#include "Game.hpp"
#include "Server.hpp"
#include "SFML/Network.hpp"

bit::ClientServerState::ClientServerState(StateStack &stack, Game* game, bool isHost)
    : bit::State(stack, game), isHost(isHost), server(NULL)
{
    if(isHost)
    {
        server = new bit::Server();
    }
}

bit::ClientServerState::~ClientServerState()
{
    if(server)
    {
        delete server;
    }
}

void bit::ClientServerState::handlePacket(sf::Int32 packetType, sf::Packet &packet)
{
}