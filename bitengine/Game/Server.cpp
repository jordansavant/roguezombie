#include "Server.hpp"
#include "RemoteClient.hpp"

bit::Server::Server()
    : snapshotId(0),
      serverPort(BIT_SERVER_PORT),
      thread(&Server::executionThread, this),
      waitingThreadEnd(false),
      isListening(false),
      clientTimeoutTime(sf::seconds(3)),
      maxConnectedClients(16),
      connectedClients(0),
      clients(1)
{
    listenerSocket.setBlocking(false);

    clients[0] = new RemoteClient();

    thread.launch();
}

bit::Server::~Server()
{
    waitingThreadEnd = true;
    thread.wait();

    for(unsigned int i=0; i < clients.size(); i++)
    {
        if(clients[i])
        {
            delete clients[i];
        }
    }
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
    sf::Packet packet;
    packet << static_cast<sf::Int32>(Server::ServerPacket::ServerUpdate);
    packet << snapshotId;
    packet = preparePacket_ServerUpdate(packet);
    sendToAllClients(packet);
}

sf::Time bit::Server::now()
{
    return clock.getElapsedTime();
}

void bit::Server::handleIncomingPackets()
{
    bool detectedTimeout = false;

    for(unsigned int i=0; i < clients.size(); i++)
    {
        RemoteClient* client = clients[i];

        if(client->isConnected)
        {
            sf::Packet packet;
            while(client->socket.receive(packet) == sf::Socket::Done)
            {
                // Handle incoming packet from client
                handlePacket(packet, *client, detectedTimeout);

                // Packet received, updatet the ping timer
                client->lastPacketTime = now();
                packet.clear();
            }

            // Check if disconnected
            if(now() >= client->lastPacketTime + clientTimeoutTime)
            {
                client->hasTimedOut = true;
                detectedTimeout = true;
            }
        }
    }

    if(detectedTimeout)
    {
        handleDisconnections();
    }
}

void bit::Server::handlePacket(sf::Packet &packet, RemoteClient &client, bool &detectedTimeout)
{
    sf::Int32 packetType;
    packet >> packetType;

    switch(packetType)
    {
        case Server::ClientPacket::Quit:

            client.hasTimedOut = true;
            detectedTimeout = true;

            break;
            
        case Server::ClientPacket::ClientInformation:

            // Confirm them
            client.isConfirmed = true;

            handlePacket_ClientInformation(packet, client);

            break;

        case Server::ClientPacket::ClientUpdate:

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
}

void bit::Server::handleConnections()
{
    // If we are not accepting connections, skip
    if(!isListening)
        return;

    // If our ready and waiting client's socket starts being awesome
    if(listenerSocket.accept(clients[connectedClients]->socket) == sf::TcpListener::Done)
    {
        RemoteClient* client = clients[connectedClients];

        // Mark the new client as ready
        client->isConnected = true;
        client->lastPacketTime = now();

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

void bit::Server::handleDisconnections()
{
    for(auto itr = clients.begin(); itr != clients.end(); )
    {
        RemoteClient* client = (*itr);

        if(client->hasTimedOut)
        {
            // Inform other clients of disconnection
            sf::Packet packet_PeerClientDisconnected;
            packet_PeerClientDisconnected << static_cast<sf::Int32>(Server::ServerPacket::PeerClientDisconnected);
            packet_PeerClientDisconnected = preparePacket_PeerClientDisconnected(packet_PeerClientDisconnected);
            sendToAllClients(packet_PeerClientDisconnected);

            // Erase client
            itr = clients.erase(itr);

            // Return to listening state
            if(connectedClients < maxConnectedClients)
            {
                clients.push_back(new RemoteClient());
                setListeningState(true);
            }
        }
        else
        {
            itr++;
        }
    }
}

void bit::Server::broadcastMessage(std::string &message)
{
    // send broadcast packet to every connected client
    for(unsigned int i=0; i < clients.size(); i++)
    {
        RemoteClient* client = clients[i];

        if(client->isConnected)
        {
            sf::Packet packet_broadcastMessage;
            packet_broadcastMessage << static_cast<sf::Int32>(Server::ServerPacket::Broadcast);
            packet_broadcastMessage << message;

            client->socket.send(packet_broadcastMessage);
        }
    }
}

void bit::Server::sendToAllClients(sf::Packet &packet)
{
    for(unsigned int i=0; i < clients.size(); i++)
    {
        RemoteClient* client = clients[i];

        if(client->isConfirmed)
        {
            client->socket.send(packet);
        }
    }
}


/**
 * Handle Incoming Client Packets
 **/

void bit::Server::handleNewClient(RemoteClient &client)
{
    // Send initialize player packet
    sf::Packet packet_InitializeSelf;
    packet_InitializeSelf << static_cast<sf::Int32>(Server::ServerPacket::InitializeSelf);
    packet_InitializeSelf = preparePacket_InitializeSelf(packet_InitializeSelf);
    client.socket.send(packet_InitializeSelf);

    // Notify all other clients of new connection
    sf::Packet packet_clientConnected;
    packet_clientConnected << static_cast<sf::Int32>(Server::ServerPacket::PeerClientConnected);
    packet_clientConnected = preparePacket_PeerClientConnected(packet_clientConnected);
    sendToAllClients(packet_clientConnected);
}

void bit::Server::handlePacket_ClientInformation(sf::Packet &packet, RemoteClient &client)
{
}

void bit::Server::handlePacket_ClientUpdate(sf::Packet &packet, RemoteClient &client)
{
}


/**
 * Prepare Outgoing Server Packets
 **/

sf::Packet& bit::Server::preparePacket_InitializeSelf(sf::Packet &packet)
{
    return packet;
}

sf::Packet& bit::Server::preparePacket_PeerClientConnected(sf::Packet &packet)
{
    return packet;
}

sf::Packet& bit::Server::preparePacket_PeerClientDisconnected(sf::Packet &packet)
{
    return packet;
}

sf::Packet& bit::Server::preparePacket_ServerUpdate(sf::Packet &packet)
{
    return packet;
}