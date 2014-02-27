#include "ClientServerState.hpp"
#include "State.hpp"
#include "StateStack.hpp"
#include "../Graphics/Camera.hpp"
#include "Game.hpp"
#include "Server.hpp"
#include "SFML/Network.hpp"
#include "SFML/System.hpp"

bit::ClientServerState::ClientServerState(StateStack &stack, Game* game, bool isHost)
    : bit::State(stack, game),
      lastSnapshotId(0),
      isHost(isHost),
      server(NULL),
      isConnected(false),
      isConfirmed(false),
      timeSinceLastPacket(sf::seconds(0.0f)),
      clientTimeout(sf::seconds(2.0f)),
      tickTimer(1.0f / BIT_SERVER_TICK_FPS)
{
}

bit::ClientServerState::~ClientServerState()
{
    if(server)
    {
        delete server;
    }
}


sf::Time bit::ClientServerState::now()
{
	return clock.getElapsedTime();
}

void bit::ClientServerState::load()
{
    // Establish whether server or client
    if(isHost)
    {
        server = newServer();
		server->start();
        ipAddress = "127.0.0.1";
        port = BIT_SERVER_PORT;
    }
    else
    {
        ipAddress = "127.0.0.1"; // swap to external ip for server
        port = BIT_SERVER_PORT;
    }

    // Connect to server
    if(socket.connect(ipAddress, port, sf::seconds(5.0f)) == sf::TcpSocket::Done)
    {
        isConnected = true;
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

    if(isConnected)
    {
        // If the game is not paused and the window is in focus
        if(!isPaused && game->isInFocus)
        {
            // Pulls items from the world into a command queue
            // Calls ->handleRealtimeInput(Command c) ??
        }

        // Handle the network input
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
                isConnected = false;
                failedConnectionClock.restart();
            }
        }

        // Send update to the server
        if(tickTimer.update(gameTime) && isConfirmed)
        {
            // Send client update packet with last acknowledged snapshot id
            sf::Packet packet;
            packet << static_cast<sf::Int32>(Server::ClientPacket::ClientUpdate) << lastSnapshotId;
            packet = preparePacket_ClientUpdate(packet);
            socket.send(packet);
        }

        // Update packet duration via gameTime
        timeSinceLastPacket += gameTime;
    }
    else if(failedConnectionClock.getElapsedTime() >= sf::seconds(5.0f))
    {
        // TODO
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
		{
            handlePacket_InitializeSelf(packet);

		    // Send information packet to confirm connection with server
			sf::Packet infoPacket;
			infoPacket << static_cast<sf::Uint32>(Server::ClientPacket::ClientInformation);
			preparePacket_ClientInformation(infoPacket);
			socket.send(infoPacket);

			// Update confirmation flag
			isConfirmed = true;

            break;
		}
        case Server::ServerPacket::PeerClientConnected:

            handlePacket_PeerClientConnected(packet);

            break;

        case Server::ServerPacket::PeerClientDisconnected:

            handlePacket_ClientDisonnected(packet);

            break;

        case Server::ServerPacket::ServerUpdate:

            // Get the snapshot id
            sf::Uint32 snapshotId;
            packet >> snapshotId;

            // If it is newer
            if(lastSnapshotId < snapshotId)
            {
                // Update our client snapshot
                lastSnapshotId = snapshotId;
                handlePacket_ServerUpdate(packet);
            }

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

void bit::ClientServerState::handlePacket_PeerClientConnected(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_ClientDisonnected(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_ServerUpdate(sf::Packet &packet)
{
}

void bit::ClientServerState::handlePacket_Shutdown(sf::Packet &packet)
{
}


/**
 * Prepare Outgoing Client Packets
 **/

sf::Packet& bit::ClientServerState::preparePacket_ClientInformation(sf::Packet &packet)
{
    return packet;
}

sf::Packet& bit::ClientServerState::preparePacket_ClientUpdate(sf::Packet &packet)
{
    return packet;
}