#include "TestServer.hpp"
#include "../bitengine/Game/Server.hpp"
#include "../bitengine/Game/RemoteClient.hpp"
#include "../bitengine/System/Output.hpp"
#include "Command.hpp"

void TestServer::load()
{
    world.load();
}

void TestServer::update(sf::Time &gameTime)
{
    world.update(gameTime);
}

// Packet Handling

void TestServer::handlePacket_ClientInformation(sf::Packet &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client information");

	world.createPlayer(client);
}

void TestServer::handlePacket_ClientUpdate(sf::Packet &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client update");

	// Get command count
	sf::Int32 commandCount;
	packet >> commandCount;

	for(unsigned int i=0; i < commandCount; i++)
	{
		// Get command
		sf::Int32 command;
		packet >> command;

		// Determine how to handle
		switch(command)
		{
			case Command::PlayerUp:
			case Command::PlayerDown:
			case Command::PlayerLeft:
			case Command::PlayerRight:
				world.handlePlayerCommand(client, static_cast<Command>(command));
				break;
		}
	}
}


// Packet Sending

sf::Packet& TestServer::preparePacket_InitializeSelf(sf::Packet &packet)
{
    bit::Output::Debug("Server prepare initialize self");

    return bit::Server::preparePacket_InitializeSelf(packet);
}

sf::Packet& TestServer::preparePacket_PeerClientConnected(sf::Packet &packet)
{
    bit::Output::Debug("Server prepare initialize client connected");

    return bit::Server::preparePacket_PeerClientConnected(packet);
}

sf::Packet& TestServer::preparePacket_PeerClientDisconnected(sf::Packet &packet)
{
    bit::Output::Debug("Server prepare initialize client disconnected");

    return bit::Server::preparePacket_PeerClientDisconnected(packet);
}

sf::Packet& TestServer::preparePacket_ServerUpdate(sf::Packet &packet)
{
    bit::Output::Debug("Server prepare server update");

    world.compileSnapshot(packet);

    return bit::Server::preparePacket_ServerUpdate(packet);
}