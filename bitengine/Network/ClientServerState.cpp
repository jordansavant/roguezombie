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
#include "../System/Output.hpp"

bit::ClientServerState::ClientServerState(StateStack &stack, Game* game, bool isClient, bool isHost)
    : bit::State(stack, game),
      clientId(0),
      lastSnapshotId(0),
      isClient(isClient),
      isHost(isHost),
      server(NULL),
      isNetworkConnected(false),
      isConfirmed(false),
      timeSinceLastPacket(sf::seconds(0.0f)),
      clientTimeout(sf::seconds(20)),
      tickTimer(1.0f / BIT_SERVER_TICK_FPS),
      awaitingDisconnect(false),
      disconnectTimer(1),
      requestCounter(0)
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
        port = getServerPort();
    }
    else if(isClient)
    {
        ipAddress = getServerIpAddress();
        port = getServerPort();
    }

    // Connect to server
    if(isClient)
    {
        if(socket.connect(ipAddress, port, sf::seconds(5.0f)) == sf::TcpSocket::Done)
        {
            bit::Output::Debug("SERVER SOCKET ESTABLISHED");
            isNetworkConnected = true;
        }
        else
        {
            failedConnectionClock.restart();
        }

        // Do not block
        socket.setBlocking(false);
    }
}

bool bit::ClientServerState::update(sf::Time &gameTime)
{
    bit::State::update(gameTime);

    if(isClient)
    {
        if(isNetworkConnected)
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
                    bit::Output::Debug("SERVER PACKET TIMEOUT");
                    isNetworkConnected = false;
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

            // Check for pending disconnect acknowledgement
            if(awaitingDisconnect && disconnectTimer.update(gameTime))
            {
                handle_DisconnectTimeout();
            }
        }
        else if(failedConnectionClock.getElapsedTime() >= sf::seconds(5.0f))
        {
            bit::Output::Debug("SERVER SOCKET CANNOT BE ESTABLISHED AFTER 5 SECONDS OF TRYING");
            handle_ServerTimeout();
        }
    }

    return true;
}

void bit::ClientServerState::disconnect()
{
    if(isClient && isNetworkConnected)
    {
        awaitingDisconnect = true;
        bit::ClientPacket packet;
        packet << static_cast<sf::Int32>(Server::ClientPacketType::Quit);
        preparePacket_ClientDisconnect(packet);
        socket.send(packet);
    }
}

void bit::ClientServerState::handlePacket(sf::Int32 packetType, bit::ServerPacket &packet)
{
    if(isClient)
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

                break;
            }
            case Server::ServerPacketType::InitializeWorld:
            {
                handlePacket_InitializeWorld(packet);

                // Update confirmation flag
                isConfirmed = true;

                break;
            }
            case Server::ServerPacketType::Event:
            {
                handlePacket_ServerEvent(packet);

                break;
            }
            case Server::ServerPacketType::Response:
            {
                // Find our request
                sf::Uint32 requestId;
                packet >> requestId;

                // Run its handling method if not abandoned
                if(!requests[requestId].isAbandonded)
                {
                    requests[requestId].onComplete(packet);
                }
                requests.erase(requestId);

                break;
            }
            case Server::ServerPacketType::DisconnectAcknowledged:
            {
                handlePacket_DisconnectAcknowledge(packet);

                break;
            }
            case Server::ServerPacketType::Kick:
            {
                handlePacket_Kick(packet);

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
}

void bit::ClientServerState::serverRequest(std::function<void(ClientPacket&)> prepare, std::function<void(ServerPacket&)> onComplete)
{
    Request request;
    request.id = ++requestCounter;
    request.onComplete = onComplete;
    requests[request.id] = request;

    bit::ClientPacket packet;
    packet << static_cast<sf::Uint32>(Server::ClientPacketType::Request);
    packet << sf::Uint32(request.id);
    prepare(packet);
    socket.send(packet);
}