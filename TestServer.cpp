#include "TestServer.hpp"
#include "bitengine/Game/Server.hpp"
#include "bitengine/Game/RemoteClient.hpp"
#include "bitengine/System/Output.hpp"

void TestServer::handlePacket_ClientEvent(sf::Packet &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle player event");
}

void TestServer::handlePacket_ClientRealtimeChange(sf::Packet &packet, bit::RemoteClient &client)
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

sf::Packet& TestServer::preparePacket_ClientConnected(sf::Packet &packet)
{
    bit::Output::Debug("Server prepare initialize client connected");
    return packet;
}

sf::Packet& TestServer::preparePacket_ClientDisconnected(sf::Packet &packet)
{
    bit::Output::Debug("Server prepare initialize client disconnected");
    return packet;
}

sf::Packet& TestServer::preparePacket_ServerUpdate(sf::Packet &packet)
{
    bit::Output::Debug("Server prepare tick update");
    return packet;
}