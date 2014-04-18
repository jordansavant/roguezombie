#include "Server.hpp"
#include "RemoteClient.hpp"
#include "ServerPacket.hpp"
#include "ClientPacket.hpp"
#include "../System/Output.hpp"

bit::Server::Server()
    : snapshotId(0),
      serverPort(0),
      thread(&Server::executionThread, this),
      waitingThreadEnd(false),
      isListening(false),
      clientTimeoutTime(sf::seconds(5)),
      maxConnectedClients(16),
      connectedClients(0),
	  clientIdentifier(0),
      clients(1)
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
    packet << static_cast<sf::Int32>(Server::ServerPacketType::Shutdown);
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
            packet << static_cast<sf::Int32>(Server::ServerPacketType::ServerUpdate);
            packet << snapshotId;
            preparePacket_ServerUpdate(packet, *client);
            client->socket.send(packet);
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
    bool detectedDisconnection = false;

    for(unsigned int i=0; i < clients.size(); i++)
    {
        RemoteClient* client = clients[i];

        if(client->isConnected)
        {
            ClientPacket packet;
            while(client->socket.receive(packet) == sf::Socket::Done)
            {
                // Handle incoming packet from client
                handlePacket(packet, *client);

                // Packet received, updatet the ping timer
                client->lastPacketTime = now();
                packet.clear();
            }

            // Check if disconnected
            if(now() >= client->lastPacketTime + clientTimeoutTime)
            {
                client->hasTimedOut = true;
                detectedDisconnection = true;

                handle_ClientTimeout(*client);
            }
        }
    }

    handleDisconnections();
}

void bit::Server::handlePacket(ClientPacket &packet, RemoteClient &client)
{
    sf::Int32 packetType;
    packet >> packetType;

    switch(packetType)
    {
        case Server::ClientPacketType::Quit:
        {
            // Mark client as disconnection received
            client.hasDisconnected = true;

            handlePacket_ClientDisconnect(packet, client);

            // Send them the quit ack packet
            bit::ServerPacket ackPacket;
            ackPacket << static_cast<sf::Int32>(Server::ServerPacketType::DisconnectAcknowledged);
            preparePacket_DisconnectAcknowledge(ackPacket, client);
            client.socket.send(ackPacket);

            break;
        }
        case Server::ClientPacketType::ClientInformation:
        {
            // Confirm them
            client.isConfirmed = true;

            handlePacket_ClientInformation(packet, client);

            if(!client.badConnection())
            {
                // Send them the full world
                bit::ServerPacket worldPacket;
                worldPacket << static_cast<sf::Int32>(Server::ServerPacketType::InitializeWorld);
                preparePacket_InitializeWorld(worldPacket, client);
                client.socket.send(worldPacket);
            }

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

void bit::Server::handleDisconnections()
{
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
                packet_PeerClientDisconnected << static_cast<sf::Int32>(Server::ServerPacketType::PeerClientDisconnected);
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

void bit::Server::broadcastMessage(std::string &message)
{
    // send broadcast packet to every connected client
    for(unsigned int i=0; i < clients.size(); i++)
    {
        RemoteClient* client = clients[i];

        if(client->isConnected)
        {
            ServerPacket packet_broadcastMessage;
            packet_broadcastMessage << static_cast<sf::Int32>(Server::ServerPacketType::Broadcast);
            packet_broadcastMessage << message;

            client->socket.send(packet_broadcastMessage);
        }
    }
}

void bit::Server::kickClient(bit::RemoteClient &client, unsigned int kickCode)
{
    bit::Output::Debug("Server prepare server kick client");

    bit::ServerPacket packet;
    packet << static_cast<sf::Int32>(Server::ServerPacketType::Kick);
    packet << sf::Uint32(kickCode);
    client.socket.send(packet);

    client.hasBeenKicked = true;
}

void bit::Server::sendToAllClients(ServerPacket &packet)
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

void bit::Server::handleNewClient(RemoteClient &client)
{
    // Send initialize player packet
    ServerPacket packet_InitializeSelf;
    packet_InitializeSelf << static_cast<sf::Int32>(Server::ServerPacketType::InitializeSelf);
    packet_InitializeSelf << sf::Uint32(client.id);
    preparePacket_InitializeSelf(packet_InitializeSelf, client);
    client.socket.send(packet_InitializeSelf);

    // Notify all other clients of new connection
    ServerPacket packet_clientConnected;
    packet_clientConnected << static_cast<sf::Int32>(Server::ServerPacketType::PeerClientConnected);
    preparePacket_PeerClientConnected(packet_clientConnected);
    sendToAllClients(packet_clientConnected);
}

void bit::Server::sendEventToClient(bit::RemoteClient &client, std::function<void(ServerPacket&)> prepare)
{
    ServerPacket packet;
    packet << static_cast<sf::Int32>(Server::ServerPacketType::Event);
    packet << sf::Uint32(client.id);

    prepare(packet);
    client.socket.send(packet);
}

void bit::Server::sendEventToAllClients(std::function<void(ServerPacket&)> prepare)
{
    ServerPacket packet;
    packet << static_cast<sf::Int32>(Server::ServerPacketType::Event);
    prepare(packet);
    sendToAllClients(packet);
}