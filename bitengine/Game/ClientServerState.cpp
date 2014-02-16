#include "ClientServerState.hpp"
#include "State.hpp"
#include "StateStack.hpp"
#include "../Graphics/Camera.hpp"
#include "Game.hpp"
#include "Server.hpp"
#include "SFML/Network.hpp"
#include "SFML/System.hpp"

bit::ClientServerState::ClientServerState(StateStack &stack, Game* game, bool isHost)
    : bit::State(stack, game), isHost(isHost), server(NULL), connected(false), timeSinceLastPacket(sf::seconds(0.0f)), clientTimeout(sf::seconds(2.0f)), tickTimer(1.0f / 20.0f)
{
    // Establish whether server or client
    if(isHost)
    {
        server = newServer();
        ipAddress = "127.0.0.1";
        port = 12345;
    }
    else
    {
        ipAddress = "127.0.0.1"; // swap to external ip for server
        port = 12345;
    }

    // Connect to server
    if(socket.connect(ipAddress, port, sf::seconds(5.0f)) == sf::TcpSocket::Done)
    {
        connected = true;
    }
    else
    {
        failedConnectionClock.restart();
    }

    // Do not block
    socket.setBlocking(false);
}

bit::ClientServerState::~ClientServerState()
{
    if(server)
    {
        delete server;
    }
}

bit::Server* bit::ClientServerState::newServer()
{
    return new bit::Server();
}

bool bit::ClientServerState::update(sf::RenderWindow &window, sf::Time &gameTime)
{
    bit::State::update(window, gameTime);

    if(connected)
    {
        // 1. Update game specific logic

        // 2. If the game is not paused and the window is in focus (Local Input?)
        if(!isPaused && game->isInFocus)
        {
            // Pulls items from the world into a command queue
            // Calls ->handleRealtimeInput(Command c) ??
        }

        // 3. Always handle the network input
        sf::Packet packet;
        if(socket.receive(packet) == sf::Socket::Done)
        {
            // Pull the header type and pass into handlePacket
            timeSinceLastPacket = sf::seconds(0.0f);
            sf::Int32 packetType;
            packet >> packetType;
            handlePacket(packetType, packet);
        }
        else
        {
            // Check for connection timeout
            if(timeSinceLastPacket > clientTimeout)
            {
                connected = false;

                // Game specific stuff for connection timeout

                failedConnectionClock.restart();
            }
        }

        // Send Updates to the server for Game Events
        // These are done the instant the message needs to be sent
        // ....

        // Send position updates
        // These are done on the communication tick
        // ....
        if(tickTimer.update(gameTime))
        {
            // send SF Packet with each positional update of the player
        }

        // Update packet duration via gameTime
        timeSinceLastPacket += gameTime;
    }
    else if(failedConnectionClock.getElapsedTime() >= sf::seconds(5.0f))
    {
        // Connection was not established, pop to previous state?
        // Override this in game code?
        requestStackPop();
    }

    return true;
}

void bit::ClientServerState::handlePacket(sf::Int32 packetType, sf::Packet &packet)
{
    switch(packetType)
    {
        case Server::ServerPacket::Broadcast:

            handlePacket_Broadcast(packet);

            break;

        case Server::ServerPacket::InitializeSelf:

            handlePacket_InitializeSelf(packet);

            break;

        case Server::ServerPacket::InitializeWorld:

            handlePacket_InitializeWorld(packet);

            break;

        case Server::ServerPacket::PeerConnected:

            handlePacket_PeerConnected(packet);

            break;

        case Server::ServerPacket::PeerDisconnected:

            handlePacket_PeerDisonnected(packet);

            break;

        case Server::ServerPacket::Update:

            handlePacket_Update(packet);

            break;

        case Server::ServerPacket::PeerEvent:

            handlePacket_PeerEvent(packet);

            break;

        case Server::ServerPacket::PeerRealtimeChange:

            handlePacket_PeerRealtimeChange(packet);

            break;

        case Server::ServerPacket::Shutdown:

            handlePacket_Shutdown(packet);

            break;
    }
}

void bit::ClientServerState::handlePacket_Broadcast(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_InitializeSelf(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_InitializeWorld(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_PeerConnected(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_PeerDisonnected(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_Update(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_PeerEvent(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_PeerRealtimeChange(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_Shutdown(sf::Packet &packet)
{
}