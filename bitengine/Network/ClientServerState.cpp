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

bit::ClientServerState::ClientServerState(StateStack &stack, Game* game, bool isClient, bool isHost, bool isLocalOnly)
    : bit::State(stack, game),
    clientId(0),
    lastSnapshotId(0),
    isClient(isClient),
    isHost(isHost),
    isLocalOnly(isLocalOnly),
    server(NULL),
    isNetworkConnected(false),
    isConfirmed(false),
    timeSinceLastPacket(sf::seconds(0.0f)),
    clientTimeout(sf::seconds(20)),
    tickTimer(1.0f / BIT_SERVER_TICK_FPS),
    awaitingDisconnect(false),
    disconnectTimer(1),
    requestCounter(0),
    direct_serverToClientPackets(),
    direct_clientToServerPackets(),
    directConnectMutex()
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
    // Threaded local connection that avoids networking
    if (isLocalOnly)
    {
        // Setup Server in a direct connection mode
        server = newServer();
        server->directClientState = this;
        server->start();

        // Setup Client in a direct connection mode
        bit::Output::Debug("SERVER DIRECT-CONNECT ESTABLISHED");
        isNetworkConnected = true;

        return;
    }

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

    // LOCAL DIRECT CONNECTION VERSION

    if (isLocalOnly)
    {
        // We are both a server and a client but without networking connections
        // We use Mutexes to transmit our packets

        // Confirm the server has loaded
        if (!hasServerLoadCompleted())
        {
            return true;
        }

        // Confirm we are setup
        if (isNetworkConnected)
        {
            ServerPacket packet;
            while (direct_receiveFromServer(packet))
            {
                // Pull the header type and pass into handlePacket
                timeSinceLastPacket = sf::seconds(0.0f);
                sf::Uint32 packetType;
                packet >> packetType;
                handlePacket(packetType, packet);
            }

            // Check for connection timeout
            if (timeSinceLastPacket > clientTimeout)
            {
                bit::Output::Debug("SERVER DIRECT-CONNECT TIMEOUT");
                isNetworkConnected = false;
                failedConnectionClock.restart();
            }

            // Send update to the server
            if (tickTimer.update(gameTime) && isConfirmed)
            {
                // Send client update packet with last acknowledged snapshot id
                bit::ClientPacket packet;
                packet << static_cast<sf::Uint32>(Server::ClientPacketType::ClientUpdate) << lastSnapshotId;
                preparePacket_ClientUpdate(packet);
                direct_clientSendToServer(packet);
            }

            // Update packet duration via gameTime
            timeSinceLastPacket += gameTime;

            // Check for pending disconnect acknowledgement
            if (awaitingDisconnect && disconnectTimer.update(gameTime))
            {
                handle_DisconnectTimeout();
            }
        }
        else if (failedConnectionClock.getElapsedTime() >= sf::seconds(5.0f))
        {
            bit::Output::Debug("SERVER DIRECT-CONNECT CANNOT BE ESTABLISHED AFTER 5 SECONDS OF TRYING");
            handle_ServerTimeout();
        }

        return true;
    }

    // NETWORK CONNECTION VERSION

    if(isHost)
    {
        // We are both a server and a client
        // It is possible that the server is still loading
        if(!hasServerLoadCompleted())
        {
            return true;
        }
    }

    if(isClient)
    {
        if(isNetworkConnected) // Socket is connected
        {
            // Handle the network input
            ServerPacket packet;
            if(socket.receive(packet) == sf::Socket::Done)
            {
                // Pull the header type and pass into handlePacket
                timeSinceLastPacket = sf::seconds(0.0f);
                sf::Uint32 packetType;
                packet >> packetType;
                handlePacket(packetType, packet);
            }
            else if(timeSinceLastPacket > clientTimeout)
            {
                bit::Output::Debug("SERVER PACKET TIMEOUT");
                isNetworkConnected = false;
                failedConnectionClock.restart();
            }

            // Send update to the server
            if(tickTimer.update(gameTime) && isConfirmed)
            {
                // Send client update packet with last acknowledged snapshot id
                bit::ClientPacket packet;
                packet << static_cast<sf::Uint32>(Server::ClientPacketType::ClientUpdate) << lastSnapshotId;
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
    if (isLocalOnly && isNetworkConnected)
    {
        awaitingDisconnect = true;
        bit::ClientPacket packet;
        packet << static_cast<sf::Uint32>(Server::ClientPacketType::Quit);
        preparePacket_ClientDisconnect(packet);
        direct_clientSendToServer(packet);
    }
    else if(isClient && isNetworkConnected)
    {
        awaitingDisconnect = true;
        bit::ClientPacket packet;
        packet << static_cast<sf::Uint32>(Server::ClientPacketType::Quit);
        preparePacket_ClientDisconnect(packet);
        socket.send(packet);
    }
}

void bit::ClientServerState::handlePacket(sf::Uint32 packetType, bit::ServerPacket &packet)
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
                if (isLocalOnly)
                {
                    direct_clientSendToServer(infoPacket);
                }
                else
                {
                    socket.send(infoPacket);
                }

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
    if (isLocalOnly)
    {
        direct_clientSendToServer(packet);
    }
    else
    {
        socket.send(packet);
    }
}

bool bit::ClientServerState::hasServerLoadCompleted()
{
    bool serverLoadComplete = false;
    server->isLoadCompleteMutex.lock();
    serverLoadComplete = server->isLoadComplete;
    server->isLoadCompleteMutex.unlock();
    return serverLoadComplete;
}

void bit::ClientServerState::direct_serverSendToClient(ServerPacket &packet)
{
    directConnectMutex.lock();
    direct_serverToClientPackets.push_back(packet);
    directConnectMutex.unlock();
}

bool bit::ClientServerState::direct_receiveFromServer(ServerPacket &packet)
{
    bool hasPacket = false;

    directConnectMutex.lock();
    if (direct_serverToClientPackets.size() > 0) {
        packet = direct_serverToClientPackets.front();
        direct_serverToClientPackets.pop_front();
        hasPacket = true;
    }
    directConnectMutex.unlock();

    return hasPacket;
}

void bit::ClientServerState::direct_clientSendToServer(ClientPacket &packet)
{
    directConnectMutex.lock();
    direct_clientToServerPackets.push_back(packet);
    directConnectMutex.unlock();
}

bool bit::ClientServerState::direct_receiveFromClient(ClientPacket &packet)
{
    bool hasPacket = false;

    directConnectMutex.lock();
    if (direct_clientToServerPackets.size() > 0) {
        packet = direct_clientToServerPackets.front();
        direct_clientToServerPackets.pop_front();
        hasPacket = true;
    }
    directConnectMutex.unlock();

    return hasPacket;
}
