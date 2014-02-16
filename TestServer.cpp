#include "TestServer.hpp"
#include "bitengine/Game/Server.hpp"
#include "bitengine/System/Output.hpp"

void TestServer::handlePacket_PlayerEvent(sf::Packet &packet, bit::Server::RemotePeer &peer)
{
    bit::Output::Debug("Server handle player event");
}

void TestServer::handlePacket_PlayerRealtimeChange(sf::Packet &packet, bit::Server::RemotePeer &peer)
{
    bit::Output::Debug("Server handle player realtime change");
}

sf::Packet& TestServer::preparePacket_InitializeSelf(sf::Packet &packet)
{
    bit::Output::Debug("Server prepare initialize self");
    return packet;
}

sf::Packet& TestServer::preparePacket_InitializeWorld(sf::Packet &packet)
{
    bit::Output::Debug("Server prepare initialize world");
    return packet;
}

sf::Packet& TestServer::preparePacket_PeerConnected(sf::Packet &packet)
{
    bit::Output::Debug("Server prepare initialize peer connected");
    return packet;
}

sf::Packet& TestServer::preparePacket_PeerDisconnected(sf::Packet &packet)
{
    bit::Output::Debug("Server prepare initialize peer disconnected");
    return packet;
}