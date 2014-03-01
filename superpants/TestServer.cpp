#include "TestServer.hpp"
#include "../bitengine/Network/Server.hpp"
#include "../bitengine/Network/RemoteClient.hpp"
#include "../bitengine/Network/ServerPacket.hpp"
#include "../bitengine/Network/ClientPacket.hpp"
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

void TestServer::handlePacket_ClientInformation(bit::ClientPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client information");

	world.createPlayer(client);
}

void TestServer::handlePacket_ClientUpdate(bit::ClientPacket &packet, bit::RemoteClient &client)
{
    bit::Output::Debug("Server handle client update");

	// Get command count
	sf::Uint32 commandCount;
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



/**
 * Prepare Outgoing Server Packets
 **/

void TestServer::preparePacket_InitializeSelf(bit::ServerPacket &packet)
{
    bit::Output::Debug("Server prepare initialize self");
}

void TestServer::preparePacket_PeerClientConnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Server prepare initialize client connected");
}

void TestServer::preparePacket_PeerClientDisconnected(bit::ServerPacket &packet)
{
    bit::Output::Debug("Server prepare initialize client disconnected");
}

void TestServer::preparePacket_ServerUpdate(bit::ServerPacket &packet)
{
    bit::Output::Debug("Server prepare server update");

    world.prepareSnapshot(packet);
}