#include "Server.hpp"
#include "RemoteClient.hpp"
#include "ServerPacket.hpp"
#include "ClientPacket.hpp"
#include "../System/Output.hpp"
#include "ClientServerState.hpp"

bit::Server::Server()
    : snapshotId(0),
      serverPort(0),
      thread(&Server::executionThread, this),
      waitingThreadEnd(false),
      isListening(false),
      clientTimeoutTime(sf::seconds(20)),
      maxConnectedClients(16),
      connectedClients(0),
      clientIdentifier(0),
      clients(1),
      isLoadComplete(false),
      directClientState(NULL)
{
    listenerSocket.setBlocking(false);

    clients[0] = new RemoteClient();

    serverPort = getServerPort();
}

bit::Server::~Server()
{
    for(unsigned int i=0; i < clients.size(); i++)
    {
        if(clients[i])
        {
            delete clients[i];
        }
    }
}

void bit::Server::start()
{
    thread.launch();
}

void bit::Server::stop()
{
    // Send server shutdown packet
    ServerPacket packet;
    packet << static_cast<sf::Uint32>(Server::ServerPacketType::Shutdown);
    preparePacket_Shutdown(packet);
    sendToAllClients(packet);

    waitingThreadEnd = true;
    thread.wait();
}

void bit::Server::setListeningState(bool state)
{
    if(state)
    {
        if(!isListening)
        {
            isListening = (listenerSocket.listen(serverPort) == sf::TcpListener::Done);
        }
    }
    else
    {
        listenerSocket.close();
        isListening = false;
    }
}

void bit::Server::executionThread()
{
    setListeningState(true);

    sf::Time stepInterval = sf::seconds(1.f / BIT_SERVER_UPDATE_FPS);
    sf::Time stepTime = sf::Time::Zero;
    sf::Time tickInterval = sf::seconds(1.f / BIT_SERVER_TICK_FPS);
    sf::Time tickTime = sf::Time::Zero;
    sf::Clock stepClock, tickClock;

    load();
    isLoadCompleteMutex.lock();
    isLoadComplete = true;
    isLoadCompleteMutex.unlock();

    while(!waitingThreadEnd)
    {
        handleIncomingPackets();
        handleConnections();

        stepTime += stepClock.getElapsedTime();
        stepClock.restart();

        tickTime += tickClock.getElapsedTime();
        tickClock.restart();

        // Fixed update step
        while(stepTime >= stepInterval)
        {
            update(stepTime);

            stepTime -= stepInterval;
        }

        // Fixed tick step
        while(tickTime >= tickInterval)
        {
            tick();

            tickTime -= tickInterval;
        }

        // Sleep
        sf::sleep(sf::milliseconds(50.0f));
    }
}

void bit::Server::load()
{
    // Update the world objects
}

void bit::Server::update(sf::Time &gameTime)
{
    // Update the world objects
}

void bit::Server::tick()
{
    // Update snapshot id
    snapshotId++;

    // Send world objects to the clients
    for(unsigned int i=0; i < clients.size(); i++)
    {
        RemoteClient* client = clients[i];

        if(client->isConfirmed)
        {
            ServerPacket packet;
            packet << static_cast<sf::Uint32>(Server::ServerPacketType::ServerUpdate);
            packet << snapshotId;
            preparePacket_ServerUpdate(packet, *client);
            if (directClientState)
            {
                // Local only
                directClientState->direct_serverSendToClient(packet);
            }
            else
            {
                // Network
                client->socket.send(packet);
            }
        }
    }
}

sf::Time bit::Server::now()
{
    return clock.getElapsedTime();
}

unsigned short bit::Server::getServerPort()
{
    return BIT_SERVER_PORT;
}

void bit::Server::handleIncomingPackets()
{
    // Traverse all available or active connections
    for(unsigned int i=0; i < clients.size(); i++)
    {
        RemoteClient* client = clients[i];

        // If this is a connected client, not a pending client slot
        if(client->isNetworkConnected)
        {
            if (directClientState)
            {
                // Local Only
                ClientPacket packet;
                while (directClientState->direct_receiveFromClient(packet))
                {
                    // Handle incoming packet from client
                    handlePacket(packet, *client);

                    // Packet received, update the ping timer
                    client->lastPacketTime = now();
                    packet.clear();
                }
            }
            else
            {
                // Network
                ClientPacket packet;
                while(client->socket.receive(packet) == sf::Socket::Done)
                {
                    // Handle incoming packet from client
                    handlePacket(packet, *client);

                    // Packet received, update the ping timer
                    client->lastPacketTime = now();
                    packet.clear();
                }

            }

            // Check if disconnected
            if(now() >= client->lastPacketTime + clientTimeoutTime)
            {
                client->hasTimedOut = true;

                handle_ClientTimeout(*client);
            }
        }
    }

    handleDisconnections();
}

void bit::Server::handleConnections()
{

    // If we are not accepting connections, skip
    if (!isListening)
        return;

    // If we are in a local only direct connection do not use networking to establish connection just direct connect to client thread
    if (directClientState)
    {
        // Mark the new client as ready
        RemoteClient* client = clients[connectedClients];
        client->isNetworkConnected = true;
        client->lastPacketTime = now();
        client->id = ++clientIdentifier;

        setListeningState(false);
        handleNewClient(*client);
    }

    // If our ready and waiting client's socket starts being awesome
    if(listenerSocket.accept(clients[connectedClients]->socket) == sf::TcpListener::Done)
    {
        RemoteClient* client = clients[connectedClients];

        // Mark the new client as ready
        client->isNetworkConnected = true;
        client->lastPacketTime = now();
        client->id = ++clientIdentifier;

        // Close connection slots or add a new one
        connectedClients++;
        if(connectedClients >= maxConnectedClients)
        {
            setListeningState(false);
        }
        else
        {
            clients.push_back(new RemoteClient());
        }

        // Add our player to the world
        handleNewClient(*client);
    }
}

void bit::Server::handlePacket(ClientPacket &packet, RemoteClient &client)
{
    sf::Uint32 packetType;
    packet >> packetType;

    switch(packetType)
    {
        case Server::ClientPacketType::ClientInformation:
        {
            // Begin processing their information
            handlePacket_ClientInformation(packet, client);

            break;
        }
        case Server::ClientPacketType::ClientUpdate:
        {
            // Update their most rescent ack
            sf::Uint32 snapshotId;
            packet >> snapshotId;
            if(client.lastAcknowledgedSnapshotId < snapshotId)
            {
                client.lastAcknowledgedSnapshotId = snapshotId;
            }

            // Always update their incoming information in case packets with input commands are late
            handlePacket_ClientUpdate(packet, client);

            break;
        }
        case Server::ClientPacketType::Request:
        {
            // Get the request ID
            sf::Uint32 requestId;
            packet >> requestId;

            // Build a response packet
            bit::ServerPacket responsePacket;
            responsePacket << static_cast<sf::Uint32>(Server::ServerPacketType::Response);
            responsePacket << requestId;

            // Allow the game to handle the request and form a response
            handlePacket_ClientRequest(packet, client, responsePacket);

            // Send the response
            if (directClientState)
            {
                directClientState->direct_serverSendToClient(responsePacket);
            }
            else
            {
                client.socket.send(responsePacket);
            }

            break;
        }
        case Server::ClientPacketType::Quit:
        {
            // Mark client as disconnection received
            client.hasDisconnected = true;

            handlePacket_ClientDisconnect(packet, client);

            // Send them the quit ack packet
            bit::ServerPacket ackPacket;
            ackPacket << static_cast<sf::Uint32>(Server::ServerPacketType::DisconnectAcknowledged);
            preparePacket_DisconnectAcknowledge(ackPacket, client);

            // Send the response
            if (directClientState)
            {
                directClientState->direct_serverSendToClient(ackPacket);
            }
            else
            {
                client.socket.send(ackPacket);
            }

            break;
        }
    }
}

void bit::Server::handleDisconnections()
{
    // Local TODO NOT SURE IF THIS WORKS
    if (directClientState)
    {
        for (auto itr = clients.begin(); itr != clients.end(); )
        {
            RemoteClient* client = (*itr);

            if (client->hasTimedOut || client->hasDisconnected || client->hasBeenKicked)
            {
                if (client->hasTimedOut)
                    bit::Output::Debug("CLIENT DIRECT-CONNECT TIMEOUT");
                else if (client->hasDisconnected)
                    bit::Output::Debug("CLIENT DIRECT-CONNECT QUIT");
                else if (client->hasBeenKicked)
                    bit::Output::Debug("CLIENT DIRECT-CONNECT KICKED");

                // Erase client
                delete client;
                itr = clients.erase(itr);

                setListeningState(false); // no more connections
            }
            else
            {
                itr++;
            }
        }

        return;
    }

    // Network
    for(auto itr = clients.begin(); itr != clients.end(); )
    {
        RemoteClient* client = (*itr);

        if(client->hasTimedOut || client->hasDisconnected || client->hasBeenKicked)
        {
            if(client->hasTimedOut)
                bit::Output::Debug("CLIENT TIMEOUT");
            else if(client->hasDisconnected)
                bit::Output::Debug("CLIENT QUIT");
            else if(client->hasBeenKicked)
                bit::Output::Debug("CLIENT KICKED");

            // Erase client
            client->socket.disconnect();
            delete client;
            itr = clients.erase(itr);

            // Return to listening state
            if(connectedClients < maxConnectedClients)
            {
                clients.push_back(new RemoteClient());
                setListeningState(true);
            }
            else
            {
                // Inform other clients of disconnection
                ServerPacket packet_PeerClientDisconnected;
                packet_PeerClientDisconnected << static_cast<sf::Uint32>(Server::ServerPacketType::PeerClientDisconnected);
                preparePacket_PeerClientDisconnected(packet_PeerClientDisconnected);
                sendToAllClients(packet_PeerClientDisconnected);
            }
        }
        else
        {
            itr++;
        }
    }
}

void bit::Server::kickClient(bit::RemoteClient &client, unsigned int kickCode)
{
    bit::Output::Debug("Server prepare server kick client");

    if(client.canReceiveGamePackets())
    {
        bit::ServerPacket packet;
        packet << static_cast<sf::Uint32>(Server::ServerPacketType::Kick);
        packet << sf::Uint32(kickCode);
        if (directClientState)
        {
            directClientState->direct_serverSendToClient(packet);
        }
        else
        {
            client.socket.send(packet);
        }

        client.hasBeenKicked = true;
    }
}

void bit::Server::sendWorldInitialization(bit::RemoteClient &client)
{
    if(!client.badConnection())
    {
        // Set the client as confirmed for communication
        client.isConfirmed = true;

        // Send them the full world
        bit::ServerPacket worldPacket;
        worldPacket << static_cast<sf::Uint32>(Server::ServerPacketType::InitializeWorld);
        preparePacket_InitializeWorld(worldPacket, client);
        if (directClientState)
        {
            directClientState->direct_serverSendToClient(worldPacket);
        }
        else
        {
            client.socket.send(worldPacket);
        }

    }
}

void bit::Server::sendToAllClients(ServerPacket &packet)
{
    // Local only
    if (directClientState)
    {
        directClientState->direct_serverSendToClient(packet);
        return;
    }

    // Network
    for(unsigned int i=0; i < clients.size(); i++)
    {
        RemoteClient* client = clients[i];

        if(client->canReceiveGamePackets())
        {
            client->socket.send(packet);
        }
    }
}

void bit::Server::handleNewClient(RemoteClient &client)
{
    // Send initialize player packet
    ServerPacket packet_InitializeSelf;
    packet_InitializeSelf << static_cast<sf::Uint32>(Server::ServerPacketType::InitializeSelf);
    packet_InitializeSelf << sf::Uint32(client.id);
    preparePacket_InitializeSelf(packet_InitializeSelf, client);

    if (directClientState)
    {
        // Local only
        directClientState->direct_serverSendToClient(packet_InitializeSelf);
    }
    else
    {
        // Network
        client.socket.send(packet_InitializeSelf);

        // Notify all other clients of new connection
        ServerPacket packet_clientConnected;
        packet_clientConnected << static_cast<sf::Uint32>(Server::ServerPacketType::PeerClientConnected);
        preparePacket_PeerClientConnected(packet_clientConnected);
        sendToAllClients(packet_clientConnected);
    }
}

void bit::Server::sendEventToClient(bit::RemoteClient &client, std::function<void(ServerPacket&)> prepare)
{
    if(client.canReceiveGamePackets())
    {
        ServerPacket packet;
        packet << static_cast<sf::Uint32>(Server::ServerPacketType::Event);
        prepare(packet);

        if (directClientState)
        {
            // Local only
            directClientState->direct_serverSendToClient(packet);
        }
        else
        {
            // Network
            client.socket.send(packet);
        }
    }
}

void bit::Server::sendEventToAllClients(std::function<void(ServerPacket&)> prepare)
{
    ServerPacket packet;
    packet << static_cast<sf::Uint32>(Server::ServerPacketType::Event);
    prepare(packet);
    sendToAllClients(packet);
}