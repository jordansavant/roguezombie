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
}

bit::ClientServerState::~ClientServerState()
{
    if(server)
    {
        delete server;
    }
}


void bit::ClientServerState::load()
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
            sf::Packet packet;
            packet << static_cast<sf::Int32>(Server::ClientPacket::ClientUpdate);
            packet = preparePacket_ClientUpdate(packet);
            socket.send(packet);
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


bit::Server* bit::ClientServerState::newServer()
{
    return new bit::Server();
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

        case Server::ServerPacket::ClientConnected:

            handlePacket_ClientConnected(packet);

            break;

        case Server::ServerPacket::ClientDisconnected:

            handlePacket_ClientDisonnected(packet);

            break;

        case Server::ServerPacket::ServerUpdate:

            handlePacket_ServerUpdate(packet);

            break;

        case Server::ServerPacket::ClientEvent:

            handlePacket_ClientEvent(packet);

            break;

        case Server::ServerPacket::ClientRealtimeChange:

            handlePacket_ClientRealtimeChange(packet);

            break;

        case Server::ServerPacket::Shutdown:

            handlePacket_Shutdown(packet);

            break;
    }
}


/**
 * Handle Incoming Server Packets
 **/

void bit::ClientServerState::handlePacket_Broadcast(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_InitializeSelf(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_InitializeWorld(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_ClientConnected(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_ClientDisonnected(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_ServerUpdate(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_ClientEvent(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_ClientRealtimeChange(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_Shutdown(sf::Packet &packet)
{
}


/**
 * Prepare Outgoing Client Packets
 **/

sf::Packet& bit::ClientServerState::preparePacket_ClientUpdate(sf::Packet &packet)
{
    return packet;
}