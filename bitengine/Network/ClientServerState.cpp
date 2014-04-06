#include "ClientServerState.hpp"
#include "../Game/State.hpp"
#include "../Game/StateStack.hpp"
#include "../Graphics/Camera.hpp"
#include "../Game/Game.hpp"
#include "Server.hpp"
#include "ServerPacket.hpp"
#include "ClientPacket.hpp"
#include "SFML/Network.hpp"
#include "SFML/System.hpp"

bit::ClientServerState::ClientServerState(StateStack &stack, Game* game, bool isHost)
    : bit::State(stack, game),
      clientId(0),
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
        server->stop();

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
        ipAddress = getServerIpAddress();
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

bool bit::ClientServerState::update(sf::Time &gameTime)
{
    bit::State::update(gameTime);

    if(isConnected)
    {
        // Handle the network input
        ServerPacket packet;
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
            bit::ClientPacket packet;
            packet << static_cast<sf::Int32>(Server::ClientPacketType::ClientUpdate) << lastSnapshotId;
            preparePacket_ClientUpdate(packet);
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

void bit::ClientServerState::handlePacket(sf::Int32 packetType, bit::ServerPacket &packet)
{
    switch(packetType)
    {
        case Server::ServerPacketType::Broadcast:

            handlePacket_Broadcast(packet);

            break;

        case Server::ServerPacketType::InitializeSelf:
		{
            packet >> clientId;
            handlePacket_InitializeSelf(packet);

		    // Send information packet to confirm connection with server
			bit::ClientPacket infoPacket;
			infoPacket << static_cast<sf::Uint32>(Server::ClientPacketType::ClientInformation);
			preparePacket_ClientInformation(infoPacket);
			socket.send(infoPacket);

			// Update confirmation flag
			isConfirmed = true;

            break;
		}
        case Server::ServerPacketType::InitializeWorld:
		{
            handlePacket_InitializeWorld(packet);

            break;
		}
        case Server::ServerPacketType::PeerClientConnected:
        {
            handlePacket_PeerClientConnected(packet);

            break;
        }
        case Server::ServerPacketType::PeerClientDisconnected:
        {
            handlePacket_ClientDisonnected(packet);

            break;
        }
        case Server::ServerPacketType::ServerUpdate:
        {
            // Get the snapshot id
            sf::Uint32 snapshotId;
            packet >> snapshotId;

            // If it is newer
            if(lastSnapshotId < snapshotId || snapshotId == 0)
            {
                // Update our client snapshot
                lastSnapshotId = snapshotId;
                handlePacket_ServerUpdate(packet);
            }

            break;
        }
        case Server::ServerPacketType::Shutdown:
        {
            handlePacket_Shutdown(packet);

            break;
        }
    }
}