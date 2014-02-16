#include "Server.hpp"

bit::Server::Server()
    : serverPort(1234),
      thread(&Server::executionThread, this),
      waitingThreadEnd(false),
      isListening(false),
      clientTimeoutTime(sf::seconds(3)),
      maxConnectedClients(16),
      connectedClients(0),
      peers(1)
{
    listenerSocket.setBlocking(false);

    peers[0] = new RemotePeer();

    thread.launch();
}

bit::Server::~Server()
{
    waitingThreadEnd = true;
    thread.wait();

    for(unsigned int i=0; i < peers.size(); i++)
    {
        if(peers[i])
        {
            delete peers[i];
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

    sf::Time stepInterval = sf::seconds(1.f / 60.f);
    sf::Time stepTime = sf::Time::Zero;
    sf::Time tickInterval = sf::seconds(1.f / 20.f);
    sf::Time tickTime = sf::Time::Zero;
    sf::Clock stepClock, tickClock;

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
            update();

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

void bit::Server::update()
{
    // Update the world objects
}

void bit::Server::tick()
{
    // Send world objects to the clients
}

sf::Time bit::Server::now()
{
    return clock.getElapsedTime();
}

void bit::Server::handleIncomingPackets()
{
    bool detectedTimeout = false;

    for(unsigned int i=0; i < peers.size(); i++)
    {
        RemotePeer* peer = peers[i];

        if(peer->isReady)
        {
            sf::Packet packet;
            while(peer->socket.receive(packet) == sf::Socket::Done)
            {
                // Handle incoming packet from client
                handlePacket(packet, *peer, detectedTimeout);

                // Packet received, updatet the ping timer
                peer->lastPacketTime = now();
                packet.clear();
            }

            // Check if disconnected
            if(now() >= peer->lastPacketTime + clientTimeoutTime)
            {
                peer->hasTimedOut = true;
                detectedTimeout = true;
            }
        }
    }

    if(detectedTimeout)
    {
        handleDisconnections();
    }
}

void bit::Server::handlePacket(sf::Packet &packet, RemotePeer &peer, bool &detectedTimeout)
{
    sf::Int32 packetType;
    packet >> packetType;

    switch(packetType)
    {
        case Server::ClientPacket::Quit:

            peer.hasTimedOut = true;
            detectedTimeout = true;

            break;

        case Server::ClientPacket::PlayerEvent:

            handlePacket_PlayerEvent(packet, peer);

            break;

        case Server::ClientPacket::PlayerRealtimeChange:

            handlePacket_PlayerRealtimeChange(packet, peer);

            break;
    }
}

void bit::Server::handleConnections()
{
    // If we are not accepting connections, skip
    if(isListening)
        return;

    // If our ready and waiting peer's socket starts being awesome
    if(listenerSocket.accept(peers[connectedClients]->socket) == sf::TcpListener::Done)
    {
        // Send world state to player
        sf::Packet packet_InitializeWorld;
        packet_InitializeWorld << static_cast<sf::Int32>(Server::ServerPacket::InitializeWorld);
        packet_InitializeWorld = preparePacket_InitializeWorld(packet_InitializeWorld);
        peers[connectedClients]->socket.send(packet_InitializeWorld);

        // Send initialize player packet
        sf::Packet packet_InitializeSelf;
        packet_InitializeSelf << static_cast<sf::Int32>(Server::ServerPacket::InitializeSelf);
        packet_InitializeSelf = preparePacket_InitializeSelf(packet_InitializeSelf);
        peers[connectedClients]->socket.send(packet_InitializeSelf);

        // Notify all other peers of new connection
        sf::Packet packet_peerConnected;
        packet_peerConnected << static_cast<sf::Int32>(Server::ServerPacket::PeerConnected);
        packet_peerConnected = preparePacket_PeerConnected(packet_peerConnected);
        sendToAllPeers(packet_peerConnected);

        // Mark the new peer as ready
        peers[connectedClients]->isReady = true;
        peers[connectedClients]->lastPacketTime = now();

        // Close connection slots or add a new one
        connectedClients++;
        if(connectedClients >= maxConnectedClients)
        {
            setListeningState(false);
        }
        else
        {
            peers.push_back(new RemotePeer());
        }
    }
}

void bit::Server::handleDisconnections()
{
    for(auto itr = peers.begin(); itr != peers.end(); )
    {
        RemotePeer* peer = (*itr);

        if(peer->hasTimedOut)
        {
            // Inform other peers of disconnection
            sf::Packet packet_PeerDisconnected;
            packet_PeerDisconnected << static_cast<sf::Int32>(Server::ServerPacket::PeerDisconnected);
            packet_PeerDisconnected = preparePacket_PeerDisconnected(packet_PeerDisconnected);
            sendToAllPeers(packet_PeerDisconnected);

            // Erase peer
            itr = peers.erase(itr);

            // Return to listening state
            if(connectedClients < maxConnectedClients)
            {
                peers.push_back(new RemotePeer());
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
    for(unsigned int i=0; i < peers.size(); i++)
    {
        RemotePeer* peer = peers[i];

        if(peer->isReady)
        {
            sf::Packet packet_broadcastMessage;
            packet_broadcastMessage << static_cast<sf::Int32>(Server::ServerPacket::Broadcast);
            packet_broadcastMessage << message;

            peer->socket.send(packet_broadcastMessage);
        }
    }
}

void bit::Server::sendToAllPeers(sf::Packet &packet)
{
    for(unsigned int i=0; i < peers.size(); i++)
    {
        RemotePeer* peer = peers[i];

        if(peer->isReady)
        {
            peer->socket.send(packet);
        }
    }
}

bit::Server::RemotePeer::RemotePeer()
: isReady(false), hasTimedOut(false)
{
    socket.setBlocking(false);
}

void bit::Server::handlePacket_PlayerEvent(sf::Packet &packet, RemotePeer &peer)
{
}

void bit::Server::handlePacket_PlayerRealtimeChange(sf::Packet &packet, RemotePeer &peer)
{
}

sf::Packet& bit::Server::preparePacket_InitializeSelf(sf::Packet &packet)
{
    return packet;
}

sf::Packet& bit::Server::preparePacket_InitializeWorld(sf::Packet &packet)
{
    return packet;
}

sf::Packet& bit::Server::preparePacket_PeerConnected(sf::Packet &packet)
{
    return packet;
}

sf::Packet& bit::Server::preparePacket_PeerDisconnected(sf::Packet &packet)
{
    return packet;
}