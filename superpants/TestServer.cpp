#include "TestServer.hpp"
#include "../bitengine/Game/Server.hpp"
#include "../bitengine/Game/RemoteClient.hpp"
#include "../bitengine/System/Output.hpp"

void TestServer::load()
{
    serverWorld.serverLoad();
}

void TestServer::update(sf::Time &gameTime)
{
    serverWorld.serverUpdate(gameTime);
}

void TestServer::handlePacket_ClientUpdate(sf::Packet &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client update");
}

void TestServer::handlePacket_ClientAcknowledge(sf::Packet &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client acknowledge");
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

sf::Packet& TestServer::preparePacket_PeerClientConnected(sf::Packet &packet)
{
    bit::Output::Debug("Server prepare initialize client connected");
    return packet;
}

sf::Packet& TestServer::preparePacket_PeerClientDisconnected(sf::Packet &packet)
{
    bit::Output::Debug("Server prepare initialize client disconnected");
    return packet;
}

sf::Packet& TestServer::preparePacket_ServerUpdate(sf::Packet &packet)
{
    serverWorld.compileSnapshot(packet);
    bit::Output::Debug("Server prepare server update");
    return packet;
}